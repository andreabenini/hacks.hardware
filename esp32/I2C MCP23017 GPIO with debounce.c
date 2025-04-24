/*
 * This file is part of andreabenini/hacks.hardware repository.
 *
 * Copyright (c) 2025 Andrea Benini
 *
 * This software is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License (CC BY-NC-SA 4.0)
 * and includes the following additional restriction:
 *
 * - The licensee is explicitly prohibited from using this software or code in any product or service that is offered for a fee,
 *   or deployed and utilized on public cloud infrastructure (including, but not limited to, AWS, Google Cloud Platform,
 *   Microsoft Azure, and similar platforms) without explicit written permission from the author.
 *
 * To view a copy of the CC BY-NC-SA 4.0 license, visit http://creativecommons.org/licenses/by-nc-sa/4.0/.
 */

/**
 * Using MCP23017 I2C chip with a ESP32C3
 *
 * - I2C setup with the new official method ("driver/i2c_master.h") is somewhat weird
 *   and tricky at the same time and it totally diverts from previous docs and forum's
 *   post on I2C should work with all ESP32 platforms
 *   This is my first attempt to make it work considering the old method is deprecated
 *   from espressif and it will be ceased soon
 * - As a working example I'll add to the mix the quite popular MCP23017 I2C I/O extender
 *   that is a nice way of adding additional I/Os to the small and cheap ESP32C3. This chip
 *   is really cool but its form factor and price pays a lot in terms of available I/O and
 *   an I2C extender is somewhat needed for dealing with multiple I/Os
 *
 */
#include <stdio.h>
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define I2C_MASTER_SCL_IO           5      // SCL pin
#define I2C_MASTER_SDA_IO           6      // SDA pin
#define I2C_MASTER_FREQ_HZ          400000 // 400kHz
#define I2C_MASTER_PORT             0      // I2C port number

#define MCP23017_ADDR               0x20    // Default address
#define MCP23017_REG_IODIRA         0x00    // I/O direction register A
#define MCP23017_REG_IODIRB         0x01    // I/O direction register B
#define MCP23017_REG_GPPUA          0x0C    // Pull-up resistor register A
#define MCP23017_REG_GPPUB          0x0D    // Pull-up resistor register B
#define MCP23017_REG_GPIOA          0x12    // GPIO register A
#define MCP23017_REG_GPIOB          0x13    // GPIO register B

#define TOTAL_BUTTONS               16      // Total number of buttons
#define DEBOUNCE_TIME_MS            50      // Debounce time in milliseconds

static const char *TAG = "mcp23017";

// Struct to track debounce state for each button
typedef struct {
    bool current_state;          // Current debounced state
    bool last_raw_state;         // Last raw state read
    uint32_t last_change_time;   // Time of last state change
} button_state_t;

static button_state_t button_states[TOTAL_BUTTONS] = {0};

// Initialize I2C master (previous implementation remains the same)
static esp_err_t init_i2c_master(i2c_master_bus_handle_t *bus_handle, i2c_master_dev_handle_t *dev_handle) {
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_MASTER_PORT,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    esp_err_t err = i2c_new_master_bus(&i2c_mst_config, bus_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C master bus install failed: %s", esp_err_to_name(err));
        return err;
    }
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = MCP23017_ADDR,
        .scl_speed_hz = I2C_MASTER_FREQ_HZ,
    };
    err = i2c_master_bus_add_device(*bus_handle, &dev_cfg, dev_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C device install failed: %s", esp_err_to_name(err));
        return err;
    }
    return ESP_OK;
}

// Read from MCP23017 register
static esp_err_t mcp23017_read_reg(i2c_master_dev_handle_t dev_handle, uint8_t reg, uint8_t *data) {
    return i2c_master_transmit_receive(dev_handle, &reg, 1, data, 1, -1);
}

// Write to MCP23017 register
static esp_err_t mcp23017_write_reg(i2c_master_dev_handle_t dev_handle, uint8_t reg, uint8_t data) {
    uint8_t write_buf[2] = {reg, data};
    return i2c_master_transmit(dev_handle, write_buf, 2, -1);
}

// Initialize all pins as inputs with pull-ups
static esp_err_t init_buttons(i2c_master_dev_handle_t dev_handle) {
    esp_err_t err;

    // Set all pins as inputs (1 = input, 0 = output)
    err = mcp23017_write_reg(dev_handle, MCP23017_REG_IODIRA, 0xFF);
    if (err != ESP_OK) return err;

    err = mcp23017_write_reg(dev_handle, MCP23017_REG_IODIRB, 0xFF);
    if (err != ESP_OK) return err;

    // Enable pull-up resistors on all pins
    err = mcp23017_write_reg(dev_handle, MCP23017_REG_GPPUA, 0xFF);
    if (err != ESP_OK) return err;

    err = mcp23017_write_reg(dev_handle, MCP23017_REG_GPPUB, 0xFF);
    if (err != ESP_OK) return err;

    return ESP_OK;
}

// Update button states with debouncing
static void update_button_states(uint8_t gpioa_data, uint8_t gpiob_data) {
    uint32_t current_time = pdTICKS_TO_MS(xTaskGetTickCount());
    
    // Process GPIOA buttons (0-7)
    for (int i = 0; i < 8; i++) {
        bool raw_state = (gpioa_data & (1 << i)) > 0;
        
        // If the raw state changed from our last reading
        if (raw_state != button_states[i].last_raw_state) {
            button_states[i].last_raw_state = raw_state;
            button_states[i].last_change_time = current_time;
        }
        // If enough time has passed since the last change, update the current state
        else if (current_time - button_states[i].last_change_time >= DEBOUNCE_TIME_MS) {
            if (button_states[i].current_state != raw_state) {
                button_states[i].current_state = raw_state;
                // Log state changes only after debouncing
                ESP_LOGI(TAG, "Button A%d state changed to: %d", i, raw_state);
            }
        }
    }

    // Process GPIOB buttons (8-15)
    for (int i = 0; i < 8; i++) {
        bool raw_state = (gpiob_data & (1 << i)) > 0;
        
        // If the raw state changed from our last reading
        if (raw_state != button_states[i + 8].last_raw_state) {
            button_states[i + 8].last_raw_state = raw_state;
            button_states[i + 8].last_change_time = current_time;
        }
        // If enough time has passed since the last change, update the current state
        else if (current_time - button_states[i + 8].last_change_time >= DEBOUNCE_TIME_MS) {
            if (button_states[i + 8].current_state != raw_state) {
                button_states[i + 8].current_state = raw_state;
                // Log state changes only after debouncing
                ESP_LOGI(TAG, "Button B%d state changed to: %d", i, raw_state);
            }
        }
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "Initializing I2C master...");
    
    i2c_master_bus_handle_t bus_handle;
    i2c_master_dev_handle_t dev_handle;
    
    // Initialize I2C
    esp_err_t err = init_i2c_master(&bus_handle, &dev_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize I2C master");
        return;
    }

    // Initialize buttons
    err = init_buttons(dev_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize buttons");
        i2c_master_bus_rm_device(dev_handle);
        i2c_del_master_bus(bus_handle);
        return;
    }

    // Main loop to read buttons
    while (1) {
        uint8_t gpioa_raw_states, gpiob_raw_states;
        
        // Read GPIOA and GPIOB
        err = mcp23017_read_reg(dev_handle, MCP23017_REG_GPIOA, &gpioa_raw_states);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to read GPIOA");
            continue;
        }

        err = mcp23017_read_reg(dev_handle, MCP23017_REG_GPIOB, &gpiob_raw_states);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to read GPIOB");
            continue;
        }

        // Invert and mask to get pressed states (0 = pressed, 1 = not pressed)
        gpioa_raw_states = ~gpioa_raw_states;
        gpiob_raw_states = ~gpiob_raw_states;
        
        // Update debounced button states
        update_button_states(gpioa_raw_states, gpiob_raw_states);

        // Optional: Periodically log all button states
        ESP_LOGI(TAG, "Debounced states: "
                 "A0:%d A1:%d A2:%d A3:%d A4:%d A5:%d A6:%d A7:%d "
                 "B0:%d B1:%d B2:%d B3:%d B4:%d B5:%d B6:%d B7:%d",
                 button_states[0].current_state, button_states[1].current_state,
                 button_states[2].current_state, button_states[3].current_state,
                 button_states[4].current_state, button_states[5].current_state,
                 button_states[6].current_state, button_states[7].current_state,
                 button_states[8].current_state, button_states[9].current_state,
                 button_states[10].current_state, button_states[11].current_state,
                 button_states[12].current_state, button_states[13].current_state,
                 button_states[14].current_state, button_states[15].current_state);

        vTaskDelay(pdMS_TO_TICKS(10)); // Poll more frequently for better responsiveness
    }

    // Cleanup (unreachable in this example)
    i2c_master_bus_rm_device(dev_handle);
    i2c_del_master_bus(bus_handle);
}
