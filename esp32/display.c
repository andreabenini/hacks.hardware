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
 * Driving a ST 7735 display from a ESP32C3 with SPI
 *
 * working example
 */
#include <stdio.h>                  // IWYU pragma: keep
#include <stdlib.h>                 // IWYU pragma: keep
#include <string.h>
#include "freertos/FreeRTOS.h"      // IWYU pragma: keep
#include "freertos/task.h"
#include "esp_system.h"             // IWYU pragma: keep
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "st7735";

// ST7735 display dimensions (in landscape mode)
#define ST7735_WIDTH  160
#define ST7735_HEIGHT 128

// Color definitions in 565 format
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// GPIO pin assignments
#define LCD_SCK     GPIO_NUM_4
#define LCD_MOSI    GPIO_NUM_6
#define LCD_CS      GPIO_NUM_7
#define LCD_A0      GPIO_NUM_5
#define LCD_RESET   GPIO_NUM_10

// ST7735 commands
#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13
#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36
#define ST7735_FRMCTR1 0xB1
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6
#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_VMCTR1  0xC5
#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

static spi_device_handle_t spi;

// Large font definition - 8x12 pixel font (custom size, more visible)
// This is a simplified large font with only the characters needed for "Hello World"
static const uint8_t large_font_H[] = {
    0x00,0x00,0x00,0x00,0xCC,0xCC,0xCC,0xFC,0xCC,0xCC,0xCC,0xCC
};

static const uint8_t large_font_e[] = {
    0x00,0x00,0x00,0x00,0xFC,0xC0,0xC0,0xF8,0xC0,0xC0,0xC0,0xFC
};

static const uint8_t large_font_l[] = {
    0x00,0x00,0x00,0x00,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xFC     // L
};

static const uint8_t large_font_o[] = {
    0x00,0x00,0x00,0x00,0x78,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x78     // O
};

static const uint8_t large_font_W[] = {
    0x00,0x00,0x00,0x00,0xCC,0xCC,0xCC,0xCC,0xFC,0xFC,0xCC,0x84
};

static const uint8_t large_font_r[] = {
    0x00,0x00,0x00,0x00,0xF8,0xCC,0xCC,0xF8,0xD8,0xCC,0xCC,0xCC
};

static const uint8_t large_font_d[] = {
    0x00,0x00,0x00,0x00,0xF0,0xD8,0xCC,0xCC,0xCC,0xCC,0xD8,0xF0
};

static const uint8_t large_font_space[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // space
    0x00, 0x00, 0x00, 0x00
};

// Send command to display
void st7735_cmd(const uint8_t cmd) {
    esp_err_t ret;
    spi_transaction_t t;
    
    // Command mode (A0/DC pin low)
    gpio_set_level(LCD_A0, 0);
    
    memset(&t, 0, sizeof(t));
    t.length = 8;                   // 8 bits
    t.tx_buffer = &cmd;             // Data
    t.user = (void*)0;              // D/C needs to be set to 0
    
    ret = spi_device_polling_transmit(spi, &t);
    assert(ret == ESP_OK);
}

// Send data to display
void st7735_data(const uint8_t *data, int len) {
    esp_err_t ret;
    spi_transaction_t t;
    
    if (len == 0) return;
    
    // Data mode (A0/DC pin high)
    gpio_set_level(LCD_A0, 1);
    
    memset(&t, 0, sizeof(t));
    t.length = len * 8;             // len bytes
    t.tx_buffer = data;             // Data
    t.user = (void*)1;              // D/C needs to be set to 1
    
    ret = spi_device_polling_transmit(spi, &t);
    assert(ret == ESP_OK);
}

// Send a single byte of data
void st7735_data_byte(uint8_t data) {
    st7735_data(&data, 1);
}

// Send a 16-bit value as data
void st7735_data_word(uint16_t data) {
    uint8_t buf[2] = {(data >> 8) & 0xFF, data & 0xFF};
    st7735_data(buf, 2);
}

// Set the display address window
void st7735_set_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    st7735_cmd(ST7735_CASET);
    st7735_data_byte(0x00);
    st7735_data_byte(x0);
    st7735_data_byte(0x00);
    st7735_data_byte(x1);
    
    st7735_cmd(ST7735_RASET);
    st7735_data_byte(0x00);
    st7735_data_byte(y0);
    st7735_data_byte(0x00);
    st7735_data_byte(y1);
    
    st7735_cmd(ST7735_RAMWR);
}

// Fill the screen with a single color
void st7735_fill_screen(uint16_t color) {
    st7735_set_window(0, 0, ST7735_WIDTH - 1, ST7735_HEIGHT - 1);
    
    for (int i = 0; i < ST7735_WIDTH * ST7735_HEIGHT; i++) {
        st7735_data_word(color);
    }
}

// Draw a pixel at the specified coordinates
void st7735_draw_pixel(uint8_t x, uint8_t y, uint16_t color) {
    if (x >= ST7735_WIDTH || y >= ST7735_HEIGHT) {
        return; // Out of bounds
    }
    
    st7735_set_window(x, y, x, y);
    st7735_data_word(color);
}

// Draw a filled rectangle
void st7735_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t color) {
    if (x >= ST7735_WIDTH || y >= ST7735_HEIGHT) {
        return;
    }
    
    // Clip the rectangle to the screen boundaries
    if ((x + w - 1) >= ST7735_WIDTH) {
        w = ST7735_WIDTH - x;
    }
    if ((y + h - 1) >= ST7735_HEIGHT) {
        h = ST7735_HEIGHT - y;
    }
    
    st7735_set_window(x, y, x + w - 1, y + h - 1);
    
    for (int i = 0; i < w * h; i++) {
        st7735_data_word(color);
    }
}

// Draw a large character bitmap
void st7735_draw_large_char(uint8_t x, uint8_t y, const uint8_t* bitmap, uint16_t fg_color, uint16_t bg_color) {
    // Character width is 8 pixels, height is 12 pixels
    st7735_set_window(x, y, x + 7, y + 11);
    
    // Draw character pixel by pixel
    for (int row = 0; row < 12; row++) {
        uint8_t line = bitmap[row];
        for (int col = 0; col < 8; col++) {
            // Check if pixel is set
            uint16_t color = (line & (0x80 >> col)) ? fg_color : bg_color;
            st7735_data_word(color);
        }
    }
}

// Draw "Hello World" with large font
void st7735_draw_hello_world(uint8_t x, uint8_t y, uint16_t fg_color, uint16_t bg_color) {
    st7735_draw_large_char(x, y, large_font_H, fg_color, bg_color);
    st7735_draw_large_char(x + 9, y, large_font_e, fg_color, bg_color);
    st7735_draw_large_char(x + 18, y, large_font_l, fg_color, bg_color);
    st7735_draw_large_char(x + 27, y, large_font_l, fg_color, bg_color);
    st7735_draw_large_char(x + 36, y, large_font_o, fg_color, bg_color);
    st7735_draw_large_char(x + 45, y, large_font_space, fg_color, bg_color);
    st7735_draw_large_char(x + 54, y, large_font_W, fg_color, bg_color);
    st7735_draw_large_char(x + 63, y, large_font_o, fg_color, bg_color);
    st7735_draw_large_char(x + 72, y, large_font_r, fg_color, bg_color);
    st7735_draw_large_char(x + 81, y, large_font_l, fg_color, bg_color);
    st7735_draw_large_char(x + 90, y, large_font_d, fg_color, bg_color);
}

// Initialize the ST7735 display
void st7735_init() {
    ESP_LOGI(TAG, "Initializing ST7735 display");
    
    // Reset the display
    gpio_set_level(LCD_RESET, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(LCD_RESET, 1);
    vTaskDelay(pdMS_TO_TICKS(100));
    
    // Software reset
    st7735_cmd(ST7735_SWRESET);
    vTaskDelay(pdMS_TO_TICKS(150));
    
    // Out of sleep mode
    st7735_cmd(ST7735_SLPOUT);
    vTaskDelay(pdMS_TO_TICKS(120));
    
    // Set color mode to 16-bit color (5-6-5)
    st7735_cmd(ST7735_COLMOD);
    st7735_data_byte(0x05);
    vTaskDelay(pdMS_TO_TICKS(10));
    
    // Frame rate control
    st7735_cmd(ST7735_FRMCTR1);
    st7735_data_byte(0x01);
    st7735_data_byte(0x2C);
    st7735_data_byte(0x2D);
    
    // Set rotation to landscape mode
    st7735_cmd(ST7735_MADCTL);
    st7735_data_byte(0xA8); // MX=1, MY=0, MV=1, BGR=1: Landscape mode (90 degrees)
    
    // Inversion control
    st7735_cmd(ST7735_INVON);
    
    // Power control
    st7735_cmd(ST7735_PWCTR1);
    st7735_data_byte(0xA2);
    st7735_data_byte(0x02);
    st7735_data_byte(0x84);
    
    st7735_cmd(ST7735_PWCTR2);
    st7735_data_byte(0xC5);
    
    // VCOM control
    st7735_cmd(ST7735_VMCTR1);
    st7735_data_byte(0x0A);
    st7735_data_byte(0x00);
    
    // Gamma adjustment positive
    st7735_cmd(ST7735_GMCTRP1);
    uint8_t gamma_pos[] = {0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00};
    st7735_data(gamma_pos, sizeof(gamma_pos));
    
    // Gamma adjustment negative
    st7735_cmd(ST7735_GMCTRN1);
    uint8_t gamma_neg[] = {0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F};
    st7735_data(gamma_neg, sizeof(gamma_neg));
    
    // Normal display mode on
    st7735_cmd(ST7735_NORON);
    vTaskDelay(pdMS_TO_TICKS(10));
    
    // Display on
    st7735_cmd(ST7735_DISPON);
    vTaskDelay(pdMS_TO_TICKS(100));
    
    ESP_LOGI(TAG, "Display initialized in landscape mode");
}

// Initialize GPIO and SPI
void setup_display() {
    ESP_LOGI(TAG, "Setting up display");
    
    // Configure GPIO pins
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LCD_A0) | (1ULL << LCD_RESET),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    
    // Configure SPI bus
    spi_bus_config_t buscfg = {
        .miso_io_num = -1,  // MISO not used
        .mosi_io_num = LCD_MOSI,
        .sclk_io_num = LCD_SCK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 16 * 320 * 2, // 16 lines of max width (320) in 16-bit colors
    };
    
    // Configure SPI device
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 24 * 1000 * 1000,  // 24 MHz
        .mode = 0,
        .spics_io_num = LCD_CS,
        .queue_size = 7,
        .flags = SPI_DEVICE_NO_DUMMY,
    };
    
    // Initialize SPI bus
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
    
    // Add device to SPI bus
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg, &spi));
    
    // Initialize display
    st7735_init();
    
    // Clear display
    st7735_fill_screen(BLACK);
    
    ESP_LOGI(TAG, "Display setup complete");
}

void draw_test_pattern() {
    ESP_LOGI(TAG, "Drawing test pattern");
    
    // Draw colored rectangles - adjusted for landscape mode
    st7735_fill_rect(0, 0, ST7735_WIDTH/2, ST7735_HEIGHT/4, RED);
    st7735_fill_rect(ST7735_WIDTH/2, 0, ST7735_WIDTH/2, ST7735_HEIGHT/4, GREEN);
    st7735_fill_rect(0, ST7735_HEIGHT/4, ST7735_WIDTH/2, ST7735_HEIGHT/4, BLUE);
    st7735_fill_rect(ST7735_WIDTH/2, ST7735_HEIGHT/4, ST7735_WIDTH/2, ST7735_HEIGHT/4, YELLOW);
    st7735_fill_rect(0, ST7735_HEIGHT/2, ST7735_WIDTH/2, ST7735_HEIGHT/4, MAGENTA);
    st7735_fill_rect(ST7735_WIDTH/2, ST7735_HEIGHT/2, ST7735_WIDTH/2, ST7735_HEIGHT/4, CYAN);
    st7735_fill_rect(0, 3*ST7735_HEIGHT/4, ST7735_WIDTH/2, ST7735_HEIGHT/4, WHITE);
    st7735_fill_rect(ST7735_WIDTH/2, 3*ST7735_HEIGHT/4, ST7735_WIDTH/2, ST7735_HEIGHT/4, BLACK);
    
    ESP_LOGI(TAG, "Test pattern complete");
}

void draw_text_test() {
    ESP_LOGI(TAG, "Drawing text test");
    
    // Create area for text - large black rectangle at the bottom
    st7735_fill_rect(0, 80, 160, 48, BLACK);
    
    // Draw "Hello World" in white on black background
    st7735_draw_hello_world(10, 85, WHITE, BLACK);
    
    // Draw "Hello World" in black on white background (inverted)
    st7735_fill_rect(10, 105, 100, 15, WHITE);
    st7735_draw_hello_world(10, 105, BLACK, WHITE);
    
    ESP_LOGI(TAG, "Text test complete");
}

void app_main(void) {
    // Initialize display
    setup_display();
    
    // Draw test pattern
    draw_test_pattern();
    
    // Draw text test
    draw_text_test();
    
    ESP_LOGI(TAG, "Main application running");
    
    // Add your application code here
    
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
