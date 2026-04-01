#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_lcd_ili9341.h"
#include "esp_lcd_touch_xpt2046.h"
#include "lvgl.h"

static const char *TAG = "keymaker";

// Display SPI pins (from host-monitor.yaml)
#define LCD_HOST               SPI2_HOST
#define PIN_NUM_LCD_SCLK       14
#define PIN_NUM_LCD_MOSI       13
#define PIN_NUM_LCD_MISO       12
#define PIN_NUM_LCD_CS         15
#define PIN_NUM_LCD_DC         2
#define PIN_NUM_LCD_RST        -1
#define PIN_NUM_BK_LIGHT       21

// Touch SPI pins (from host-monitor.yaml)
#define TOUCH_HOST             SPI3_HOST
#define PIN_NUM_TOUCH_SCLK     25
#define PIN_NUM_TOUCH_MOSI     32
#define PIN_NUM_TOUCH_MISO     39
#define PIN_NUM_TOUCH_CS       33
#define PIN_NUM_TOUCH_IRQ      36

// Display resolution
#define LCD_H_RES              320
#define LCD_V_RES              240
#define LCD_PIXEL_CLOCK_HZ     (20 * 1000 * 1000)

// LVGL settings
#define LVGL_TICK_PERIOD_MS    2
#define LVGL_BUFFER_HEIGHT     40

static lv_disp_draw_buf_t disp_buf;
static lv_disp_drv_t disp_drv;
static lv_indev_drv_t indev_drv;
static lv_color_t *buf1 = NULL;
static lv_color_t *buf2 = NULL;

static esp_lcd_panel_handle_t panel_handle = NULL;
static esp_lcd_touch_handle_t touch_handle = NULL;

static SemaphoreHandle_t lvgl_mux = NULL;

// Global UI elements for touch test
static lv_obj_t *g_coord_label = NULL;

// LVGL flush callback
static void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    esp_lcd_panel_handle_t panel = (esp_lcd_panel_handle_t)drv->user_data;
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;

    esp_lcd_panel_draw_bitmap(panel, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
    lv_disp_flush_ready(drv);
}

// LVGL input device read callback
static void lvgl_touch_cb(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    esp_lcd_touch_handle_t touch = (esp_lcd_touch_handle_t)drv->user_data;
    uint16_t touch_x[1];
    uint16_t touch_y[1];
    uint16_t touch_strength[1];
    uint8_t touch_cnt = 0;

    esp_lcd_touch_read_data(touch);
    bool touched = esp_lcd_touch_get_coordinates(touch, touch_x, touch_y, touch_strength, &touch_cnt, 1);

    if (touched && touch_cnt > 0) {
        data->point.x = touch_x[0];
        data->point.y = touch_y[0];
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

// LVGL tick timer callback
static void lvgl_tick_timer_cb(void *arg)
{
    lv_tick_inc(LVGL_TICK_PERIOD_MS);
}

// LVGL task
static void lvgl_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting LVGL task");

    while (1) {
        if (xSemaphoreTake(lvgl_mux, portMAX_DELAY) == pdTRUE) {
            lv_timer_handler();
            xSemaphoreGive(lvgl_mux);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "Keymaker - LVGL Hello World");

    // Initialize backlight
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << PIN_NUM_BK_LIGHT
    };
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));
    gpio_set_level(PIN_NUM_BK_LIGHT, 0); // Off initially

    // Initialize Display SPI bus
    spi_bus_config_t lcd_bus_cfg = {
        .sclk_io_num = PIN_NUM_LCD_SCLK,
        .mosi_io_num = PIN_NUM_LCD_MOSI,
        .miso_io_num = PIN_NUM_LCD_MISO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = LCD_H_RES * LCD_V_RES * sizeof(uint16_t),
    };
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &lcd_bus_cfg, SPI_DMA_CH_AUTO));

    // Initialize LCD panel IO
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config = {
        .cs_gpio_num = PIN_NUM_LCD_CS,
        .dc_gpio_num = PIN_NUM_LCD_DC,
        .spi_mode = 0,
        .pclk_hz = LCD_PIXEL_CLOCK_HZ,
        .trans_queue_depth = 10,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle));

    // Initialize LCD panel (panel order *_BGR|*_RGB)
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_NUM_LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_ili9341(io_handle, &panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, false));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, true, false));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    // Initialize Touch SPI bus
    spi_bus_config_t touch_bus_cfg = {
        .sclk_io_num = PIN_NUM_TOUCH_SCLK,
        .mosi_io_num = PIN_NUM_TOUCH_MOSI,
        .miso_io_num = PIN_NUM_TOUCH_MISO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 0,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(TOUCH_HOST, &touch_bus_cfg, SPI_DMA_CH_AUTO));

    // Initialize touch panel IO
    esp_lcd_panel_io_handle_t touch_io_handle = NULL;
    esp_lcd_panel_io_spi_config_t touch_io_config = ESP_LCD_TOUCH_IO_SPI_XPT2046_CONFIG(PIN_NUM_TOUCH_CS);
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)TOUCH_HOST, &touch_io_config, &touch_io_handle));

    // Initialize touch controller
    esp_lcd_touch_config_t touch_config = {
        .x_max = LCD_H_RES,
        .y_max = LCD_V_RES,
        .rst_gpio_num = -1,
        .int_gpio_num = PIN_NUM_TOUCH_IRQ,
        .levels = {
            .reset = 0,
            .interrupt = 0,
        },
        .flags = {
            .swap_xy = 1,
            .mirror_x = 1,
            .mirror_y = 1,
        },
    };
    ESP_ERROR_CHECK(esp_lcd_touch_new_spi_xpt2046(touch_io_handle, &touch_config, &touch_handle));

    // Turn on backlight
    gpio_set_level(PIN_NUM_BK_LIGHT, 1);
    ESP_LOGI(TAG, "Display and touch initialized");

    // Initialize LVGL
    lv_init();

    // Create LVGL mutex
    lvgl_mux = xSemaphoreCreateMutex();
    assert(lvgl_mux);

    // Allocate LVGL draw buffers
    size_t buffer_size = LCD_H_RES * LVGL_BUFFER_HEIGHT * sizeof(lv_color_t);
    buf1 = heap_caps_malloc(buffer_size, MALLOC_CAP_DMA);
    assert(buf1);
    buf2 = heap_caps_malloc(buffer_size, MALLOC_CAP_DMA);
    assert(buf2);

    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, LCD_H_RES * LVGL_BUFFER_HEIGHT);

    // Register display driver
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LCD_H_RES;
    disp_drv.ver_res = LCD_V_RES;
    disp_drv.flush_cb = lvgl_flush_cb;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.user_data = panel_handle;
    lv_disp_t *disp = lv_disp_drv_register(&disp_drv);

    // Register input device driver
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = lvgl_touch_cb;
    indev_drv.user_data = touch_handle;
    lv_indev_drv_register(&indev_drv);

    // Create LVGL tick timer
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &lvgl_tick_timer_cb,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, LVGL_TICK_PERIOD_MS * 1000));

    ESP_LOGI(TAG, "LVGL initialized");

    // Create UI - Touch Test with proper colors
    if (xSemaphoreTake(lvgl_mux, portMAX_DELAY) == pdTRUE) {
        lv_obj_t *scr = lv_disp_get_scr_act(disp);

        // Set background to black (simple and clean)
        lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), 0);

        // Create a title label
        lv_obj_t *title = lv_label_create(scr);
        lv_label_set_text(title, "Keymaker - Touch Test");
        lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);  // White
        lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
        lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

        // Create instruction label
        lv_obj_t *instr = lv_label_create(scr);
        lv_label_set_text(instr, "Touch the screen\nto test");
        lv_obj_set_style_text_color(instr, lv_color_hex(0xAAAAAA), 0);  // Gray
        lv_obj_set_style_text_align(instr, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align(instr, LV_ALIGN_CENTER, 0, -30);

        // Create a label to show touch coordinates (store globally)
        g_coord_label = lv_label_create(scr);
        lv_label_set_text(g_coord_label, "X: --- Y: ---");
        lv_obj_set_style_text_color(g_coord_label, lv_color_hex(0x00FF00), 0);  // Green
        lv_obj_align(g_coord_label, LV_ALIGN_CENTER, 0, 10);

        // Create touch counter label
        lv_obj_t *counter = lv_label_create(scr);
        lv_label_set_text(counter, "Touches: 0");
        lv_obj_set_style_text_color(counter, lv_color_hex(0xFFFF00), 0);  // Yellow
        lv_obj_align(counter, LV_ALIGN_BOTTOM_MID, 0, -10);

        ESP_LOGI(TAG, "Touch test UI created");
        xSemaphoreGive(lvgl_mux);
    }

    // Start LVGL task
    xTaskCreate(lvgl_task, "lvgl_task", 4096, NULL, 5, NULL);

    ESP_LOGI(TAG, "Setup complete! Touch the screen to test...");

    // Touch monitoring loop in main task
    uint32_t touch_count = 0;
    bool was_touched = false;

    while (1) {
        uint16_t touch_x[1];
        uint16_t touch_y[1];
        uint16_t touch_strength[1];
        uint8_t touch_cnt = 0;

        esp_lcd_touch_read_data(touch_handle);
        bool touched = esp_lcd_touch_get_coordinates(touch_handle, touch_x, touch_y, touch_strength, &touch_cnt, 1);

        if (touched && touch_cnt > 0) {
            // Only log and update on new touch (debouncing)
            if (!was_touched) {
                touch_count++;
                ESP_LOGI(TAG, "Touch #%lu: X=%d, Y=%d, Strength=%d",
                         touch_count, touch_x[0], touch_y[0], touch_strength[0]);

                if (xSemaphoreTake(lvgl_mux, pdMS_TO_TICKS(10)) == pdTRUE) {
                    // Update coordinate label
                    if (g_coord_label) {
                        static char coord_text[32];
                        snprintf(coord_text, sizeof(coord_text), "X: %d  Y: %d", touch_x[0], touch_y[0]);
                        lv_label_set_text(g_coord_label, coord_text);
                    }

                    // Update touch counter
                    lv_obj_t *scr = lv_disp_get_scr_act(disp);
                    lv_obj_t *counter = lv_obj_get_child(scr, 3); // 4th child is the counter
                    if (counter) {
                        static char count_text[32];
                        snprintf(count_text, sizeof(count_text), "Touches: %lu", touch_count);
                        lv_label_set_text(counter, count_text);
                    }

                    xSemaphoreGive(lvgl_mux);
                }
            }
            was_touched = true;
        } else {
            was_touched = false;
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
