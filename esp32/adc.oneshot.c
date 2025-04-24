//
// Reading ADC channels, oneshot. This is a working example
//
// Benefits
//  - it works
//  - up to date functions, using the new method
//  - quite simple to understand
//
// Dislikes
//  - single_mode. oneshot functions, not optimized for continous readings
//  - slow
//
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_adc/adc_continuous.h"
#include "esp_adc/adc_oneshot.h"
#include "hal/adc_types.h"

#include "esp_adc/adc_cali.h"


void app_main(void) {
    //esp_err_t ret;
    int adc_read0, adc_read1;
    int mv_output;

    adc_oneshot_unit_handle_t handle = NULL;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &handle));
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(handle, ADC_CHANNEL_2, &config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(handle, ADC_CHANNEL_3, &config));
    adc_cali_handle_t cali_handle = NULL; 
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&cali_config, &cali_handle));
    //adc_cali_handle_t cali_handle = NULL; 
    while(1) {
        ESP_ERROR_CHECK(adc_oneshot_read(handle, ADC_CHANNEL_2, &adc_read0));
        ESP_ERROR_CHECK(adc_oneshot_read(handle, ADC_CHANNEL_3, &adc_read1));
        adc_cali_raw_to_voltage(cali_handle, adc_read1, &mv_output);
        printf("channel-2(%d), channel-3(%d)   ADC mV(%d)\n", adc_read0, adc_read1, mv_output);
        vTaskDelay(100);
    }
}
