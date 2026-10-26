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


