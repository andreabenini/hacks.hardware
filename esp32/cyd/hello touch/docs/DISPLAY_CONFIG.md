# Display Configuration Reference

## Working Configuration for CYD (ILI9341 + XPT2046)
This document contains the **final working configuration** after extensive testing.


## Display Hardware (ILI9341)
### Pin Configuration
```c
// Display SPI pins (SPI2_HOST)
#define LCD_HOST               SPI2_HOST
#define PIN_NUM_LCD_SCLK       14
#define PIN_NUM_LCD_MOSI       13
#define PIN_NUM_LCD_MISO       12
#define PIN_NUM_LCD_CS         15
#define PIN_NUM_LCD_DC         2
#define PIN_NUM_LCD_RST        -1
#define PIN_NUM_BK_LIGHT       21
```
### Display Settings
```c
esp_lcd_panel_dev_config_t panel_config = {
    .reset_gpio_num = PIN_NUM_LCD_RST,
    .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,  // RGB (NOT BGR!)
    .bits_per_pixel = 16,
};

// After panel init:
esp_lcd_panel_invert_color(panel_handle, false);  // NO inversion
esp_lcd_panel_mirror(panel_handle, true, false);
```
### LVGL Configuration (sdkconfig)
```
CONFIG_LV_COLOR_DEPTH_16=y
CONFIG_LV_COLOR_DEPTH=16
CONFIG_LV_COLOR_16_SWAP=y  # CRITICAL - Must be enabled!
```


## Touch Hardware (XPT2046)
### Pin Configuration
```c
// Touch SPI pins (SPI3_HOST)
#define TOUCH_HOST             SPI3_HOST
#define PIN_NUM_TOUCH_SCLK     25
#define PIN_NUM_TOUCH_MOSI     32
#define PIN_NUM_TOUCH_MISO     39
#define PIN_NUM_TOUCH_CS       33
#define PIN_NUM_TOUCH_IRQ      36
```
### Touch Settings
```c
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
```


## Color Testing Results
### What Works
- **RGB order**: `LCD_RGB_ELEMENT_ORDER_RGB`
- **Color invert**: `false`
- **LVGL byte swap**: `CONFIG_LV_COLOR_16_SWAP=y`
### Color Verification
```c
// Test these to verify correct configuration:
lv_color_hex(0x000000) // Black      ✓
lv_color_hex(0xFFFFFF) // White      ✓
lv_color_hex(0xFF0000) // Red        ✓
lv_color_hex(0x00FF00) // Green      ✓
lv_color_hex(0x0000FF) // Blue       ✓
lv_color_hex(0xFFFF00) // Yellow     ✓
lv_color_hex(0x00FFFF) // Cyan       ✓
lv_color_hex(0xFF00FF) // Magenta    ✓
```


## What We Tried (and didn't work)
| Setting                         | Result |
|---------------------------------|--------|
| BGR order + no invert + no swap | Red/Blue swapped |
| RGB order + no swap             | Red/Blue swapped |
| BGR order + invert              | Light background (inverted) |
| RGB order + invert + swap       | Light background (inverted) |
**Final Solution**: RGB + no invert + byte swap


## Files to Keep
### Critical Files
1. **`sdkconfig`** - Contains LVGL color settings
2. **`main/main.c`** - Display/touch initialization code
3. **`docs/DISPLAY_CONFIG.md`** - This file (reference)

### Optional But Useful
5. **`docs/SETUP.md`** - Full project setup guide
6. **`.vscode/`** - VSCode configuration (if using)


## How to Recreate This Setup
1. **Ensure correct sdkconfig settings:**
   ```bash
   grep LV_COLOR_16_SWAP sdkconfig
   # Should show: CONFIG_LV_COLOR_16_SWAP=y
   ```
2. **Verify display init in main.c:**
   - RGB order (not BGR)
   - No color inversion
   - Mirror X enabled
3. **Test with simple colors first:**
   - Black background
   - White/colored text
   - Verify no color swapping
4. **If colors are wrong:**
   - Red↔Blue swapped? Toggle `LV_COLOR_16_SWAP`
   - Light when should be dark? Toggle `invert_color`
   - Try RGB vs BGR order


## Quick Troubleshooting

### Problem: Colors are swapped (red appears blue)
**Solution**: Toggle `CONFIG_LV_COLOR_16_SWAP` in sdkconfig

### Problem: Background is inverted (light when should be dark)
**Solution**: Toggle `esp_lcd_panel_invert_color()` setting

### Problem: Touch not responding
**Solution**: Check touch SPI bus (different from display!)

### Problem: Touch coordinates wrong
**Solution**: Adjust `swap_xy`, `mirror_x`, `mirror_y` flags


## Reference Hardware

Based on CYD (Cheap Yellow Display):
- **MCU**: ESP32-WROOM-32
- **Display**: 2.8" ILI9341 (320x240, SPI)
- **Touch**: XPT2046 (resistive, SPI)
- **Additional**: SD card, RGB LED, LDR

Original working reference: `contrib/host-monitor.yaml`


## Notes

- The display requires **two separate SPI buses** (SPI2 for LCD, SPI3 for touch)
- LVGL byte swap is required due to how the ILI9341 expects RGB565 data
- Pin configuration matches the CYD hardware exactly
- Black background (#000000) works best for this display
- Last verified: 2026-03  
- ESP-IDF version: 5.5.3  
- LVGL version: 8.4.0  
