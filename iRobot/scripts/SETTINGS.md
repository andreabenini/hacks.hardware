# Roomba Operational Settings & MQTT API Reference
Every configuration option available in the iRobot mobile app corresponds to an MQTT payload sent to the Roomba's local broker on port `8883`.


## 1. Quick CLI Management (`manage_roomba.py`)
All commands read credentials automatically from `CREDENTIALS.txt`.

### Inspect Current Configuration
```sh
./manage_roomba.py --get
```

### Configure Cleaning Preferences
```sh
# Cleaning Passes
./manage_roomba.py --passes 1
./manage_roomba.py --passes 2
./manage_roomba.py --passes auto

# Edge Cleaning
./manage_roomba.py --edge-clean on
./manage_roomba.py --edge-clean off

# Full Bin Behavior
./manage_roomba.py --bin-pause on    # Pause when full
./manage_roomba.py --bin-pause off   # Always finish

# Carpet Boost (for models with Carpet Boost hardware)
./manage_roomba.py --carpet-boost auto
./manage_roomba.py --carpet-boost performance
./manage_roomba.py --carpet-boost eco
```

### Operational Commands
```bash
./manage_roomba.py --cmd start   # Start cleaning
./manage_roomba.py --cmd pause   # Pause cleaning
./manage_roomba.py --cmd stop    # Stop cleaning
./manage_roomba.py --cmd dock    # Return to base
./manage_roomba.py --cmd find    # Play chime to locate
```

---