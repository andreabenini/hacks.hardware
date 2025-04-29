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
 * - Create two tasks: task1, task2
 * - Signal from task2 to task1 once a long async operation has been completed
 * - React on task1 once the signal has been received, continute doing something else otherwise
 *
 */
 #include <stdio.h>
 #include "freertos/FreeRTOS.h"              // IWYU pragma: keep
 #include "freertos/task.h"
 #include "esp_system.h"                     // IWYU pragma: keep
 #include "esp_log.h"
 
 
 #define TAG_TASK    "main"
 #define TAG_TASK1   "task1"
 #define TAG_TASK2   "task2"
 
 
 // Task handle for Task1
 TaskHandle_t task1_handle = NULL;
 
 // Task1: Non-blocking task that checks for notifications from Task2
 void task1(void *pvParameters) {
     // Initialize notification value
     uint32_t notification_value = 0;
     BaseType_t notification_received;
     
     // Clear any initial notifications that might be present
     xTaskNotifyStateClear(NULL);
     
     // Do regular Task1 operations
     ESP_LOGI(TAG_TASK1, "Task1: Starting regular operations...");
     while (1) {
         
         // Non-blocking check for notification from Task2
         notification_received = xTaskNotifyWait(
             0,                  // Don't clear bits on entry
             ULONG_MAX,          // Clear all bits on exit
             &notification_value, // Store notification value here
             0                   // Don't wait (non-blocking)
         );
         // Debug output to confirm notification state
         // ESP_LOGI(TAG_TASK1, "Task1: Notification check result: %s", (notification_received == pdTRUE) ? "RECEIVED" : "NONE");
         
         if (notification_received == pdTRUE) {
             // Notification received - process Task2's completion
             ESP_LOGW(TAG_TASK1, "Notification received from Task2! Value: %lu", (unsigned long int) notification_value);
             // Process the work related to Task2's completion
             ESP_LOGI(TAG_TASK1, "    - Processing the results from Task2...");
             // Add your Task2 result processing code here
         // } else {
         //     ESP_LOGI(TAG_TASK1, "No notification from Task2 yet");
         }
         // // Continue with Task1's main operations
         // ESP_LOGI(TAG_TASK1, "Continuing with main operations...");
         
         // Simulate Task1's regular work
         vTaskDelay(1000 / portTICK_PERIOD_MS);
     }
 } /**/
 
 // Task2: The task that performs a long operation and notifies Task1 when done
 void task2(void *pvParameters) {
     // Add a delay to ensure Task1 is fully initialized before sending notifications
     vTaskDelay(2000 / portTICK_PERIOD_MS);
     
     while (1) {
         // Simulate a long operation
         ESP_LOGI(TAG_TASK2, "Starting long operation");
         
         // Simulate processing time (replace with your actual long operation)
         // vTaskDelay(10000 / portTICK_PERIOD_MS);
         vTaskDelay(pdMS_TO_TICKS(10000));     // POLL_DELAY between reads (as .h define)
         
         ESP_LOGI(TAG_TASK2, "    - Long operation completed, notifying Task1");
         
         // Notify Task1 that the operation is complete
         // Using eSetBits notification action which is less likely to have initialization issues
         BaseType_t notify_result = xTaskNotify(task1_handle, 1, eSetBits);
         ESP_LOGI(TAG_TASK2, "    - Notification sent to Task1 (result: %s)", (notify_result == pdPASS) ? "SUCCESS" : "FAILED");
         
         // Add some delay before starting the next operation
         vTaskDelay(2000 / portTICK_PERIOD_MS);
     }
 } /**/
 
 void app_main(void) {
     ESP_LOGI(TAG_TASK, "Starting application...");
     
     // Create Task1 with priority 2 and save its handle
     BaseType_t task1_created = xTaskCreate(task1, "Task1", 2048, NULL, 2, &task1_handle);
     
     if (task1_created != pdPASS || task1_handle == NULL) {
         ESP_LOGI(TAG_TASK, "ERROR: Failed to create Task1");
         return;
     }
     
     // Small delay to ensure Task1 is running before creating Task2
     vTaskDelay(500 / portTICK_PERIOD_MS);
     
     // Create Task2 with priority 2
     BaseType_t task2_created = xTaskCreate(task2, "Task2", 2048, NULL, 2, NULL);
     
     if (task2_created != pdPASS) {
         ESP_LOGI(TAG_TASK, "ERROR: Failed to create Task2");
         return;
     }
     ESP_LOGI(TAG_TASK, "Tasks created successfully");
 } /**/
 