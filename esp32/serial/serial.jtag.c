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
 * This program creates a task that listens for incoming input data from the SerialPort/JTAG connector.
 * Dumps output as ASCII and Hex and it can be used as a serial monitor. It's also possible to keep ESP-IDF Monitor open while sending
 * data from another program in the same port. sdkconfig file has to be properly configured for enabling Serial R/W
 *                # UART Specifications, enable UART R/W options
 *                #CONFIG_ESP_CONSOLE_UART_DEFAULT=y
 *                CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG=y
 */
/**
 * As a bonus point this sample is provided as a standalone task, it's easy to integrate in something else
 */
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"              // IWYU pragma: keep
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/usb_serial_jtag.h"

static const char *TAG = "serial_monitor";

// Buffer size for receiving data
#define BUF_SIZE 128

// Task function to receive data from USB Serial/JTAG port
void serial_rx_task(void *pvParameters) {
    // Buffer for incoming data
    uint8_t data[BUF_SIZE];
    
    ESP_LOGI(TAG, "USB Serial/JTAG monitor task started");
    
    // Configure USB Serial/JTAG driver
    usb_serial_jtag_driver_config_t usb_serial_jtag_config;
    usb_serial_jtag_config.rx_buffer_size = 1024;
    usb_serial_jtag_config.tx_buffer_size = 1024;
    
    // Install USB Serial/JTAG driver with the configuration
    ESP_ERROR_CHECK(usb_serial_jtag_driver_install(&usb_serial_jtag_config));
    
    // Main task loop
    while (1) {
        // Read data from USB Serial/JTAG
        // Based on your error message, the function takes 3 arguments
        int length = usb_serial_jtag_read_bytes(data, BUF_SIZE - 1, pdMS_TO_TICKS(100));
        
        // If data is received, log it
        if (length > 0) {
            data[length] = '\0'; // Null-terminate for string operations
            ESP_LOGI(TAG, "Received %d bytes: %s", length, data);
            
            // For binary data or to show hex values:
            ESP_LOGI(TAG, "HEX dump:");
            for (int i = 0; i < length; i++) {
                printf("%02X ", data[i]);
                if ((i + 1) % 16 == 0) {
                    printf("\n");
                }
            }
            if (length % 16 != 0) {
                printf("\n");
            }
        }
        
        // Small delay to prevent CPU hogging
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    
    // This point should never be reached
    vTaskDelete(NULL);
} /**/

void app_main(void) {
    // Give the system a moment to initialize
    vTaskDelay(pdMS_TO_TICKS(500));
    
    ESP_LOGI(TAG, "Initializing USB Serial/JTAG Monitor");
    
    // Create the serial reception task
    xTaskCreate(serial_rx_task, "serial_rx_task", 4096, NULL, 5, NULL);
    
    ESP_LOGI(TAG, "Serial Monitor initialized, now receiving data");
    
    // app_main can continue with other initialization work or just return
} /**/
