#  HELLO CYD - QUICKSTART
Minimal ESP-IDF project for CYD hardware (ILI9341 + XPT2046)

## Build and Flash
```sh
cd hello_cyd
. $IDF_PATH/export.sh
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor

# (Press Ctrl+] to exit monitor)
```

### What you'll see
Display:
- Black background
- White title: "Keymaker - Touch Test"
- Green coordinates (updates on touch)
- Yellow counter (increments on touch)

Serial:
- `I (xxx) keymaker: Touch #1: X=160, Y=120, Strength=1234`


## Hardware pins
| Description    | SCLK | MOSI |         |       |        |
|----------------|------|------|---------|-------|--------|
| Display (SPI2) | 14   | 13   | CS=15   | DC=2  | BL=21
| Touch (SPI3)   | 25   | 32   | MISO=39 | CS=33 | IRQ=36


## CRITICAL SETTINGS
- In `sdkconfig`  
  `CONFIG_LV_COLOR_16_SWAP=y`    ← MUST BE ENABLED!
- In main.c
  ```c
    .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB  // (NOT BGR!)
    esp_lcd_panel_invert_color(panel_handle, false)
  ```


## FILES INCLUDED
```
hello_cyd/
├── CMakeLists.txt               ← Project file
├── sdkconfig                    ← Config (critical!)
├── main/
│   ├── CMakeLists.txt
│   ├── idf_component.yml        ← Auto-downloads LVGL, ILI9341
│   └── main.c                   ← Touch test app
├── components/
│   └── esp_lcd_touch_xpt2046/   ← Touch driver
└── docs/
    └── DISPLAY_CONFIG.md        ← Complete reference
```

## CUSTOMIZE
- Colors:  `main.c`, line ~240 (lv_color_hex values)
- Pins:    `main.c`, lines 20-35
- Add UI:  `main.c`, line ~238 (app_main function)


## MORE INFO
- README.md              ← Full documentation
- docs/DISPLAY_CONFIG.md ← Hardware troubleshooting
