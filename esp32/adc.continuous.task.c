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

 #include <string.h>
 #include <stdio.h>
 #include "sdkconfig.h"
 #include "esp_log.h"
 #include "freertos/FreeRTOS.h"
 #include "freertos/task.h"
 #include "freertos/semphr.h"
 #include <driver/gpio.h>
 #include "esp_adc/adc_continuous.h"
 
 #define ADC_CONV_MODE                       ADC_CONV_SINGLE_UNIT_1
 #define ADC_ATTEN                           ADC_ATTEN_DB_12
 
 #if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
 #define ADC_OUTPUT_TYPE                     ADC_DIGI_OUTPUT_FORMAT_TYPE1
 #define ADC_GET_CHANNEL(p_data)     ((p_data)->type1.channel)
 #define ADC_GET_DATA(p_data)        ((p_data)->type1.data)
 #else
 #define ADC_OUTPUT_TYPE                     ADC_DIGI_OUTPUT_FORMAT_TYPE2
 #define ADC_GET_CHANNEL(p_data)     ((p_data)->type2.channel)
 #define ADC_GET_DATA(p_data)        ((p_data)->type2.data)
 #endif
 
 #define TIME_POLL_DELAY         10                  // Delay between two reads
 #define ADC_BUFFER_SIZE                      60
 
 #define JOYSTICK1_X             ADC_CHANNEL_0       // GPIO0
 #define JOYSTICK1_Y             ADC_CHANNEL_1       // GPIO1
 #define JOYSTICK2_X             ADC_CHANNEL_2       // GPIO2
 #define JOYSTICK2_Y             ADC_CHANNEL_3       // GPIO3
 
 
 static TaskHandle_t s_task_handle;
 #define TAG_KEYBOARD    "keyboard"
 
 static bool IRAM_ATTR s_conv_done_cb(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data) {
     BaseType_t mustYield = pdFALSE;
     //Notify that ADC continuous driver has done enough number of conversions
     vTaskNotifyGiveFromISR(s_task_handle, &mustYield);
     return (mustYield == pdTRUE);
 }
 
 void adcInitContinuous(adc_channel_t *channel, uint8_t channelNumber, adc_continuous_handle_t *outputHandle) {
     adc_continuous_handle_t handle = NULL;
     adc_continuous_handle_cfg_t adcConfig = {
         .max_store_buf_size = 1024,
         .conv_frame_size = ADC_BUFFER_SIZE,
     };
     ESP_LOGI(TAG_KEYBOARD, "        - Registering new handle");
     esp_err_t err = adc_continuous_new_handle(&adcConfig, &handle);
     if (err != ESP_OK) {
         ESP_LOGE(TAG_KEYBOARD, "Error while creating a new handle for ADC continuous mode: %d", err);
         *outputHandle = NULL;
         return;
     }
     adc_continuous_config_t dig_cfg = {
         .sample_freq_hz = 20 * 1000,
         .conv_mode = ADC_CONV_MODE,
         .format = ADC_OUTPUT_TYPE,
     };
     adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};
     dig_cfg.pattern_num = channelNumber;
     for (int i=0; i<channelNumber; i++) {
         adc_pattern[i].atten = ADC_ATTEN;
         adc_pattern[i].channel = channel[i] & 0x7;
         adc_pattern[i].unit = ADC_UNIT_1;
         adc_pattern[i].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;
         ESP_LOGI(TAG_KEYBOARD, "adc_pattern[%d].atten is :%"PRIx8, i, adc_pattern[i].atten);
         ESP_LOGI(TAG_KEYBOARD, "adc_pattern[%d].channel is :%"PRIx8, i, adc_pattern[i].channel);
         ESP_LOGI(TAG_KEYBOARD, "adc_pattern[%d].unit is :%"PRIx8, i, adc_pattern[i].unit);
     }
     dig_cfg.adc_pattern = adc_pattern;
     ESP_LOGI(TAG_KEYBOARD, "        - Setting continuous config mode (Channels:%d)", channelNumber);
     ESP_ERROR_CHECK(adc_continuous_config(handle, &dig_cfg));
     ESP_LOGI(TAG_KEYBOARD, "        - adc_continuous_config() set");
     *outputHandle = handle;
 }
 
 
 esp_err_t joystickInit(adc_channel_t (*channel)[4], adc_continuous_handle_t *adcHandle) {
     ESP_LOGI(TAG_KEYBOARD, "Joystick ADC Init");
     gpio_set_direction(GPIO_NUM_0, GPIO_MODE_INPUT);        // J2 Y
     gpio_set_direction(GPIO_NUM_1, GPIO_MODE_INPUT);        // J2 X
     gpio_set_direction(GPIO_NUM_2, GPIO_MODE_INPUT);        // J1 Y
     gpio_set_direction(GPIO_NUM_3, GPIO_MODE_INPUT);        // J1 X
     s_task_handle = xTaskGetCurrentTaskHandle();
     ESP_LOGI(TAG_KEYBOARD, "    - ADC init, continuous mode.  adcInitContinuous()");
     adcInitContinuous(*channel, sizeof(*channel)/sizeof(adc_channel_t), adcHandle);
     adc_continuous_evt_cbs_t cbs = {
         .on_conv_done = s_conv_done_cb,
     };
     ESP_LOGI(TAG_KEYBOARD, "    - register event callbacks");
     ESP_ERROR_CHECK(adc_continuous_register_event_callbacks(*adcHandle, &cbs, NULL));
     ESP_LOGI(TAG_KEYBOARD, "    - set ADC in continuous start mode");
     esp_err_t err = (adc_continuous_start(*adcHandle));
     ESP_ERROR_CHECK(err);
     ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
     vTaskDelay(pdMS_TO_TICKS(TIME_POLL_DELAY));             // set initial delay
     return err;
 } /**/
 
 
 void taskKeyboard(void *pvParameter) {
     ESP_LOGI(TAG_KEYBOARD, "Task Created");
     esp_err_t err;
     adc_channel_t channel[4] = {JOYSTICK1_X, JOYSTICK1_Y, JOYSTICK2_X, JOYSTICK2_Y};
     adc_continuous_handle_t adcHandle = NULL;
     uint32_t adcResultNum = 0;
     uint8_t adcData[ADC_BUFFER_SIZE] = {0};    // Continuous read adc data stream
     adc_digi_output_data_t  *p;                 // data stream pointer
     
     err = joystickInit(&channel, &adcHandle);
     if (err != ESP_OK) {
         ESP_LOGE(TAG_KEYBOARD, "Failed to initialize joystick configuration (%d)", err);
         vTaskDelete(NULL);
         return;
     }
     uint16_t X1 = 0, Y1 = 0, X2 = 0, Y2 = 0;
     /**
      * This is to show you the way to use the ADC continuous mode driver event callback.
      * This `ulTaskNotifyTake` will block when the data processing in the task is fast.
      * However in this example, the data processing (print) is slow, so you barely block here.
      *
      * Without using this event callback (to notify this task), you can still just call
      * `adc_continuous_read()` here in a loop, with/without a certain block timeout.
      */
     // Initial notification taken in joystickInit
     // ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
     while (1) {
         err = adc_continuous_read(adcHandle, adcData, ADC_BUFFER_SIZE, &adcResultNum, 0);
         if (err == ESP_OK) {
             // ESP_LOGI("TASK", "ret is %x, adcResultNum is %"PRIu32" bytes", ret, adcResultNum);
             for (int i = 0; i < adcResultNum; i += SOC_ADC_DIGI_RESULT_BYTES) {
                 p = (adc_digi_output_data_t*)&adcData[i];
                 /* Check the channel number validation, the data is invalid if the channel num exceed the maximum channel */
                 #define ADC_GET_DATA(p_data) ((p_data)->type2.data)
                 switch (ADC_GET_CHANNEL(p)) {
                     case 0:
                         X1 = ADC_GET_DATA(p);
                         break;
                     case 1:
                         Y1 = ADC_GET_DATA(p);
                         break;
                     case 2:
                         X2 = ADC_GET_DATA(p);
                         break;
                     case 3:
                         Y2 = ADC_GET_DATA(p);
                         break;
                 }
 
                 // switch (ADC_GET_CHANNEL(p)) {
                 //     case 0:
                 //         keyboard.joystick1_X = ADC_GET_DATA(p);
                 //         break;
                 //     case 1:
                 //         keyboard.joystick1_Y = ADC_GET_DATA(p);
                 //         break;
                 //     case 2:
                 //         keyboard.joystick2_X = ADC_GET_DATA(p);
                 //         break;
                 //     case 3:
                 //         keyboard.joystick2_Y = ADC_GET_DATA(p);
                 //         break;
                 // }
 
             }
             ESP_LOGI(TAG_KEYBOARD, "X1:%d, Y1:%d, X2:%d, Y2:%d", X1, Y1, X2, Y2);
             /**
              * Because printing is slow, so every time you call `ulTaskNotifyTake`, it will immediately return.
              * To avoid a task watchdog timeout, add a delay here. When you replace the way you process the data,
              * usually you don't need this delay (as this task will block for a while).
              *
              * vTaskDelay(1);
              */
         } else if (err == ESP_ERR_TIMEOUT) {
             ESP_LOGW(TAG_KEYBOARD, "ADC Continuous read timeout");
             // Try to read `ADC_BUFFER_SIZE` until API returns timeout, which means there's no available data
             // ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
         }
         vTaskDelay(pdMS_TO_TICKS(TIME_POLL_DELAY));
     }
     ESP_ERROR_CHECK(adc_continuous_stop(adcHandle));
     ESP_ERROR_CHECK(adc_continuous_deinit(adcHandle));
     vTaskDelete(NULL);                                  // Delete the task, without this I'm getting a guru meditation error with core0 in panic mode
 } /**/
 
 
 void app_main(void) {
     ESP_LOGW(TAG_KEYBOARD, "Program Started");
     // Create the button task
     xTaskCreate(taskKeyboard, "taskKeyboard", 4096, NULL, 5, NULL);
     return;
 } /**/
 