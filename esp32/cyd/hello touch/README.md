# Hello CYD - Reference Project
**Clean, minimal ESP-IDF project for CYD (Cheap Yellow Display) hardware**

This is a **working reference** for ILI9341 display + XPT2046 touch screen on ESP32.

---

## What This Is

A minimal "Hello World" for CYD hardware that demonstrates:
- Display working with correct colors
- Touch screen responding
- LVGL 8.3 integration
- Touch coordinates displayed in real-time
- Touch counter

**Use this as a starting point for any CYD-based project!**


## Project structure
```txt
hello_cyd/
├── CMakeLists.txt              # Root project file
├── sdkconfig                   # ESP-IDF config (color settings)
├── main/
│   ├── CMakeLists.txt          # Main component
│   ├── idf_component.yml       # Dependencies (LVGL, ILI9341, etc.)
│   └── main.c                  # Touch test application
├── components/
│   └── esp_lcd_touch_xpt2046/  # Touch driver (complete)
└── docs/
    └── DISPLAY_CONFIG.md       # Hardware reference
```


## Quick Start

### Prerequisites
- ESP-IDF 5.x installed
- CYD hardware (ESP32, ILI9341, XPT2046)

### Build & Flash
During the first build all dependencies (LVGL, ILI9341 driver) will automatically be downloaded
from `idf_component.yml`.
  ```sh
  # Set environment
  . $IDF_PATH/export.sh

  # Build
  cd hello_cyd
  idf.py build

  # Flash (adjust port if needed)
  idf.py -p /dev/ttyUSB0 flash monitor
  ```


## Program overview
**On the display:**
- Black background
- White title: "DemoTest - Touch Test"
- Gray text: "Touch the screen to test"
- Green coordinates (updates when touched)
- Yellow counter (increments on touch)

**On serial monitor:**
```
I (xxx) demotest: Touch #1: X=160, Y=120, Strength=1234
I (xxx) demotest: Touch #2: X=200, Y=150, Strength=2345
```


## Hardware Configuration
### Pin Mapping
| Display (ILI9341) | SPI2     |
|-------------------|----------|
| **Pin**           | **GPIO** |
| SCLK              |       14 |
| MOSI              |       13 |
| MISO              |       12 |
| CS                |       15 |
| DC                |        2 |
| Backlight         |       21 |

| Touch (XPT2046)   | SPI3     |
|-------------------|----------|
| **Pin**           | **GPIO** |
| SCLK              |       25 |
| MOSI              |       32 |
| MISO              |       39 |
| CS                |       33 |
| IRQ               |       36 |

### Critical Settings
**These settings in `sdkconfig` are CRITICAL:**
```
CONFIG_LV_COLOR_DEPTH_16=y
CONFIG_LV_COLOR_16_SWAP=y    # Must be enabled for correct colors!
```
**In `main.c`:**
```c
.rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB        // NOT BGR!
esp_lcd_panel_invert_color(panel_handle, false)   // NO inversion
```
See `docs/DISPLAY_CONFIG.md` for complete details


## Customization
### Change Colors
Edit `main.c` around line 240:
```c
// Background
lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), 0);              // Black
// Text colors
lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);          // White
lv_obj_set_style_text_color(g_coord_label, lv_color_hex(0x00FF00), 0);  // Green
```
### Change Pin Mapping
Edit `main.c` if your hardware uses different pins.
### Add Your Code
The LVGL UI is created in `app_main()`. Add your widgets there!

## Documentation
- **`docs/DISPLAY_CONFIG.md`** - Complete hardware reference
  - Pin configurations
  - Color settings troubleshooting
  - What we tried and what works


## Troubleshooting
- Colors are wrong (red appears blue)  
  **Fix:** Check `sdkconfig` has `CONFIG_LV_COLOR_16_SWAP=y`
- Display is inverted (light when should be dark)  
  **Fix:** Check `main.c` has `esp_lcd_panel_invert_color(panel_handle, false)`
- Touch not responding  
  **Fix:** Verify touch uses SPI3 (different bus than display!)
- Build fails - missing LVGL  
  **Fix:** Delete `build/` and `managed_components/`, then rebuild. Dependencies will re-download.


## External links
- **LVGL Documentation:** https://docs.lvgl.io/8.3/
- **ESP-IDF Documentation:** https://docs.espressif.com/projects/esp-idf/


## Template project
Copy `hello_cyd/` and build on top of it, this project is a clean starting point for:
- Smart displays
- Touch interfaces
- IoT dashboards
- Any CYD-based project!
