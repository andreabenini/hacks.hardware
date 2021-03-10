_cheap and dirty getting started guide for a 5min flashing, read: https://tasmota.github.io/docs/Getting-Started/ for a detailed one_

# Initial setup
Use a TTL-USB converter, cheapest and newest ones already provides 3v3 instead of classic 5v so you may be fine with them,
carefully check out your connector to be sure to provide the right voltage

### Programming Connector
| GND   | TX           | RX    | 3v3 | pinout |
|-------|--------------|-------|-----|--------|
| black | white/yellow | green | red | colors |

### Software needed
- **esptool.py**, available on github but also in your favorite Linux Distro
    - `pacman -S esptool` [Arch]
- 

# Programming mode
Press and keep pressed the button before connecting your USB cable so the Sonoff Basic might be powered and enters in programming mode
when it boots. If you see something like:
```sh
~# esptool.py --port /dev/ttyUSB0 flash_id
esptool.py v3.0
Serial port /dev/ttyUSB0
Connecting........_____....._____....._____....._____....._____....._____....._____

A fatal error occurred: Failed to connect to Espressif device: Timed out waiting for packet header
```
You're not doing the right thing. Remove the usb cable, press the sonoff pushbutton (and keep it pressed) and insert the usb cable again.

## Getting device id and flash information
When you're set you'll see something like:
```sh
~# esptool.py --port /dev/ttyUSB0 flash_id
esptool.py v3.0
Serial port /dev/ttyUSB0
Connecting....
Detecting chip type... ESP8266
Chip is ESP8285
Features: WiFi, Embedded Flash
Crystal is 26MHz
MAC: dc:4f:22:f5:da:79
Uploading stub...
Running stub...
Stub running...
Manufacturer: 51
Device: 4014
Detected flash size: 1MB
Hard resetting via RTS pin...
```
At the end of each single operation it's better to release/insert the usb cable, boot/programming mode won't last a lot, you just 
have few seconds after boot for doing something.

# Flashing
## Erasing previous firmware
I usually don't keep a backup copy of the original firmware, I've never used it and I simply don't care.
Open source firmware on my devices is the only thing I need.  
Erasing flash is a mandatory operation before flashing tasmota or something else, this removes ITEAD flash settings and their firmware
```sh
~# esptool.py --port /dev/ttyUSB0 erase_flash
esptool.py v3.0
Serial port /dev/ttyUSB0
Connecting....
Detecting chip type... ESP8266
Chip is ESP8285
Features: WiFi, Embedded Flash
Crystal is 26MHz
MAC: dc:4f:22:f5:da:79
Uploading stub...
Running stub...
Stub running...
Erasing flash (this may take a while)...
Chip erase completed successfully in 5.4s
Hard resetting via RTS pin...
```

## Flashing new firmware
As easy as it gets...  
Tasmota firmware example: (https://github.com/arendst/Tasmota)
```sh
~# esptool.py --port /dev/ttyUSB0 write_flash -fs 1MB -fm dout 0x0 tasmota.bin
esptool.py v3.0
Serial port /dev/ttyUSB0
Connecting....
Detecting chip type... ESP8266
Chip is ESP8285
Features: WiFi, Embedded Flash
Crystal is 26MHz
MAC: dc:4f:22:f5:da:79
Uploading stub...
Running stub...
Stub running...
Configuring flash size...
Compressed 580480 bytes to 399923...
Wrote 580480 bytes (399923 compressed) at 0x00000000 in 36.2 seconds (effective 128.2 kbit/s)...
Hash of data verified.

Leaving...
Hard resetting via RTS pin...
```
Remove and reinsert the USB cable again, there's no need of external mains voltage, just keep basic 3v3
for initial configuration

# First configuration
Use your favorite wireless device (pc, phone, tablet, ...) to scan wireless networks. You'll see a `tasmota*`
network (basically an hot spot), just connect to it and wait for a while. When wireless connection is set just go to this url
```
192.168.4.1
```
and configure the wireless network accordingly. Sonoff Basic will reboot and hotspot will be shut down, if
you're doing things fine you'll probably see it connected to your backbone.

That's it.

