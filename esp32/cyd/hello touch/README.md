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

