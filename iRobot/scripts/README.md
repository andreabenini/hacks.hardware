# Roomba 9xx Series Headless Setup & Integration Guide
How to happily keep your Linux system and ditch every Android app or generic cloud crapware, even if you were told you needed it.  
Provision and extract credentials from a Roomba (e.g. 9xx/96x/97s/98x) without using the proprietary iRobot mobile app.


## Physical Roomba Button Cheat Sheet
| Action                       | Physical Buttons                      | Duration  | LED / Sound Indicator                | Purpose                                         |
| :--------------------------- | :------------------------------------ | --------: | :----------------------------------- | :---------------------------------------------- |
| **Wake Up**                  | Press **CLEAN** once                  | 1 tap     | Lights turn on                       | Awakens robot from standby                      |
| **Factory Reset**            | Hold **Dock + Spot + Clean** together | 10–15 sec | All LEDs illuminate, chime plays     | Wipes all previous owner data/network settings  |
| **SoftAP Setup Mode**        | Hold **Dock + Spot** together         | 2–5 sec   | Chime plays, Wi-Fi LED blinks green  | Broadcasts ad-hoc setup Wi-Fi (Roomba-BLID)     |
| **Pairing / Password Query** | Hold **Dock / Home** alone (on base)  | ~2 sec    | 4-tone chime plays, Wi-Fi LED blinks | Authorizes local client on LAN to read password |


## Prerequisites & Environment Setup
Modern Linux distributions use OpenSSL 3.0, which disables legacy renegotiation and small DH keys by default. Roomba firmware uses older ciphers (AES128-SHA256).
1. Install requirements in a virtual environment:
   ```bash
   python3 -m venv venv
   ./venv/bin/pip install 'paho-mqtt<2.0.0'
   ```
2. The scripts automatically apply `openssl_legacy.cnf` located in this folder to allow the handshake without system-wide changes.


## End-to-End Onboarding Procedure
### Step A: Reset and Start SoftAP
1. Place the Roomba on or off the Home Base.
2. Hold **Dock + Spot + Clean** for 10–15s until all lights turn on, then release to factory reset.
3. Wake it up with **CLEAN**.
4. Hold **Dock + Spot** for ~3 seconds until the chime sounds and the Wi-Fi icon flashes green.

### Step B: Connect Host to Roomba SoftAP
1. Turn on your PC Wi-Fi and scan networks:
   ```sh
   nmcli dev wifi list
   ```
   Look for `Roomba-<BLID>` (e.g. `Roomba-92B5A36401234567`).
2. Connect your Wi-Fi interface to the Roomba:
   ```sh
   nmcli dev wifi connect Roomba-<BLID> ifname <wifi_interface>
   ```
   The Roomba assigns your host IP `192.168.10.2` via DHCP; the robot is `192.168.10.1`.

### Step C: Provision Wi-Fi Credentials
Run `provision_wifi.py`:
```sh
./venv/bin/python3 provision_wifi.py
```
- It prompts interactively for target 2.4 GHz SSID and Wi-Fi password (or pass `-s SSID -p PASSWORD`).
- It generates a valid local MQTT password (`:1:timestamp:16_chars`), registers it with the robot, and sends the Wi-Fi profile over MQTT topics `delta` and `wifictl`.
- The Roomba disables its SoftAP (`uap: false`) and connects to your home Wi-Fi.

### Step D: Reconnect Host & Confirm LAN Access
1. Reconnect your host Wi-Fi back to your home network:
   ```sh
   nmcli dev wifi connect <YOUR_HOME_SSID> ifname <wifi_interface>
   ```
2. Check `CREDENTIALS.txt` for the assigned password and BLID.
3. Verify connection with `check_status.py`:
   ```bash
   ./venv/bin/python3 check_status.py
   ```

## Retrieving the Password Later (If Already on LAN)
If the robot is already on your Wi-Fi and you just need its password:
- Ensure the robot is docked and awake
- Run `get_password.py`:
   ```sh
   ./venv/bin/python3 get_password.py
   ```
- When prompted, press and hold **Dock / Home** for 2 seconds until the chime plays and Wi-Fi blinks. The script will capture and display the password


## Setting Preferences & Controlling from CLI
All operational preferences normally adjusted in the mobile app are MQTT JSON deltas published to topic `delta` or commands sent to topic `cmd`.  
Use `manage_roomba.py` to inspect and configure them:
### View Current Settings:
```sh
./venv/bin/python3 manage_roomba.py --get
```

