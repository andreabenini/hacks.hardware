# Required Files to Reproduce Build
This is the **complete list** of files needed to rebuild this project from scratch.


## Critical Files (Must Have)
### Project Configuration
- **`CMakeLists.txt`** - Root project file
- **`sdkconfig`** - ESP-IDF configuration (includes LVGL color settings!)
- **`main/CMakeLists.txt`** - Main component build file
- **`main/idf_component.yml`** - Component dependencies (LVGL, ILI9341, etc.)
### Source Code
- **`main/main.c`** - Application code with display/touch init
### Local Components
- **`components/esp_lcd_touch_xpt2046/`** - Complete directory (custom touch driver)
  - `CMakeLists.txt`
  - `esp_lcd_touch_xpt2046.c`
  - `include/esp_lcd_touch_xpt2046.h`
  - `idf_component.yml`
  - `Kconfig.projbuild`


## VSCode Configuration (For IDE Support)
- **`.vscode/settings.json`** - ESP-IDF extension settings
- **`.vscode/c_cpp_properties.json`** - IntelliSense configuration
- **`.vscode/tasks.json`** - Build/flash tasks
- **`.vscode/launch.json`** - Debug configuration


## Documentation (Reference Only)
- **`docs/DISPLAY_CONFIG.md`** - Display/touch settings reference
- **`docs/SETUP.md`** - Setup instructions (optional)
- **`docs/VSCODE.md`** - VSCode guide (optional)
- **`contrib/host-monitor.yaml`** - Hardware reference (optional)


## Ignored files
These are auto-generated or downloaded:
- `build/` - Build output (generated)
- `managed_components/` - Downloaded from idf_component.yml
- `sdkconfig.old` - Backup file
- `.git/` - Git history (optional)


## Minimal File List (rebuild)
If you want the **absolute minimum** to reproduce:
```
keymaker/
├── CMakeLists.txt                          ← Required
├── sdkconfig                               ← Required (CRITICAL!)
├── main/
│   ├── CMakeLists.txt                      ← Required
│   ├── idf_component.yml                   ← Required
│   └── main.c                              ← Required
├── components/
│   └── esp_lcd_touch_xpt2046/              ← Required (entire folder)
│       ├── CMakeLists.txt
│       ├── esp_lcd_touch_xpt2046.c
│       ├── Kconfig.projbuild
│       ├── idf_component.yml
│       └── include/
│           └── esp_lcd_touch_xpt2046.h
└── docs/
    └── DISPLAY_CONFIG.md                   ← Reference only
```


## Complete File List (Including VSCode)
For full VSCode support:
```
keymaker/
├── CMakeLists.txt                          ← Project
├── sdkconfig                               ← Config (CRITICAL!)
├── main/
│   ├── CMakeLists.txt                      ← Build
│   ├── idf_component.yml                   ← Dependencies
│   └── main.c                              ← Source
├── components/
│   └── esp_lcd_touch_xpt2046/              ← Touch driver (all files)
│       ├── CMakeLists.txt
│       ├── esp_lcd_touch_xpt2046.c
│       ├── Kconfig.projbuild
│       ├── idf_component.yml
│       └── include/
│           └── esp_lcd_touch_xpt2046.h
├── .vscode/                                ← VSCode config
│   ├── settings.json
│   ├── c_cpp_properties.json
│   ├── tasks.json
│   └── launch.json
├── docs/                                   ← Documentation
│   ├── DISPLAY_CONFIG.md                   ← Critical reference!
│   ├── SETUP.md
│   └── VSCODE.md
└── contrib/
    └── host-monitor.yaml                   ← Hardware reference
```


## How to Rebuild from These Files
1. **Copy files to new directory**
2. **Set ESP-IDF environment:**
   ```bash
   . /home/ben/.espressif/v5.5.3/esp-idf/export.sh
   ```
3. **Build:**
   ```bash
   idf.py build
   ```
4. **Flash:**
   ```bash
   idf.py -p /dev/ttyUSB0 flash
   ```
The `idf_component.yml` will automatically download:
- LVGL 8.3
- esp_lcd_ili9341
- esp_lcd_touch


## Critical Settings in sdkconfig
**Verify these exist in your sdkconfig:**
```
CONFIG_LV_COLOR_DEPTH_16=y
CONFIG_LV_COLOR_16_SWAP=y    ← CRITICAL for correct colors!
```
**If missing**, colors will be wrong!


## Backup Strategy
### Option 1: Git Repository
```bash
git init
git add CMakeLists.txt sdkconfig main/ components/ .vscode/ docs/
git commit -m "Working display + touch configuration"
```
### Option 2: Archive
```bash
tar -czf keymaker-backup.tar.gz \
  CMakeLists.txt \
  sdkconfig \
  main/ \
  components/ \
  .vscode/ \
  docs/ \
  contrib/
```
### Option 3: Copy to Cloud
Upload these directories to backup:
- `main/`
- `components/`
- `.vscode/` (optional)
- `docs/`
- And these files:
  - `CMakeLists.txt`
  - `sdkconfig`


## Verification After Restore
1. **Build succeeds:**
   ```sh
   idf.py build
   ```
2. **Colors are correct:**
   - Black background (not light)
   - White title (not blue)
   - Green coordinates (not purple)
3. **Touch works:**
   - Coordinates update on touch
   - Counter increments


## Quick Checklist
Before backing up, verify you have:
- [ ] `sdkconfig` (with LV_COLOR_16_SWAP=y)
- [ ] `main/main.c` (with RGB order, no invert)
- [ ] `main/idf_component.yml` (dependencies list)
- [ ] `main/CMakeLists.txt`
- [ ] Root `CMakeLists.txt`
- [ ] `components/esp_lcd_touch_xpt2046/` (complete)
- [ ] `.vscode/` folder (for IDE support)
- [ ] `docs/DISPLAY_CONFIG.md` (reference)
