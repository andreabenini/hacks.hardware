#!/usr/bin/env python3
import os
import sys

# Ensure OpenSSL legacy renegotiation configuration is loaded
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
CONF_PATH = os.path.join(SCRIPT_DIR, "openssl_legacy.cnf")
if os.path.exists(CONF_PATH) and os.environ.get("OPENSSL_CONF") != CONF_PATH:
    os.environ["OPENSSL_CONF"] = CONF_PATH
    os.execv(sys.executable, [sys.executable] + sys.argv)

import binascii
import getpass
import json
import logging
import random
import socket
import ssl
import string
import time
import argparse
import paho.mqtt.client as mqtt


HOST = "192.168.10.1"
PORT = 8883
MAGIC = bytes.fromhex("efcc3b2900")


logging.basicConfig(level=logging.INFO, format="%(asctime)s [%(levelname)s] %(message)s")
log = logging.getLogger("RoombaProvision")

def create_ssl_context():
    ctx = ssl.create_default_context()
    ctx.check_hostname = False
    ctx.verify_mode = ssl.CERT_NONE
    ctx.set_ciphers("AES128-SHA256:@SECLEVEL=0")
    return ctx

def generate_roomba_password():
    ts = int(time.time())
    rand_chars = ''.join(random.choices(string.ascii_letters + string.digits, k=16))
    return f":1:{ts}:{rand_chars}"

def provision_password(password_str):
    pwd_bytes = password_str.encode("ascii")
    payload = MAGIC + pwd_bytes
    packet = bytes([0xf0, len(payload)]) + payload
    ctx = create_ssl_context()
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(10)
    wrapped = ctx.wrap_socket(sock)
    log.info(f"Connecting to Roomba at {HOST}:{PORT} over TLS...")
    wrapped.connect((HOST, PORT))
    log.info("Sending password registration packet...")
    wrapped.sendall(packet)
    resp = b""
    while len(resp) < 37:
        chunk = wrapped.recv(1024)
        if not chunk:
            break
        resp += chunk
    wrapped.close()
    if len(resp) < 7:
        raise RuntimeError("Response too short from Roomba")
    if resp[0] != 0xf0 or resp[6] == 0x03:
        raise RuntimeError(f"Robot rejected password handshake: {resp.hex()}")
    log.info("Password accepted by Roomba.")
    return True

def provision_wifi(blid, wifi_ssid, wifi_psk, roomba_pwd, timezone="Europe/Rome", country="IT"):
    ctx = create_ssl_context()
    client = mqtt.Client(client_id=blid, protocol=mqtt.MQTTv311)
    client.tls_set_context(ctx)
    client.tls_insecure_set(True)
    client.username_pw_set(blid, roomba_pwd)
    log.info(f"Connecting MQTT client with BLID={blid}...")
    client.connect(HOST, PORT, keepalive=60)
    client.loop_start()
    time.sleep(1)
    ssid_hex = binascii.hexlify(wifi_ssid.encode("utf-8")).decode("ascii")
    commands = [
        ("delta", {"state": {"timezone": timezone}}),
        ("delta", {"state": {"country": country}}),
        ("wifictl", {"state": {"wlcfg": {"pass": wifi_psk, "sec": 7, "ssid": ssid_hex}}}),
        ("wifictl", {"state": {"chkssid": True}}),
        ("wifictl", {"state": {"wactivate": True}}),
        ("wifictl", {"state": {"get": "netinfo"}}),
        ("wifictl", {"state": {"uap": False}}),
    ]
    for topic, payload in commands:
        body = json.dumps(payload)
        log.info(f"Publishing to {topic}...")
        client.publish(topic, body)
        time.sleep(1.5)
    client.loop_stop()
    client.disconnect()
    log.info("Wi-Fi configuration sent. Roomba will now shut down SoftAP and join your home Wi-Fi.")


def main():
    parser = argparse.ArgumentParser(description="Provision Wi-Fi credentials on iRobot Roomba without the mobile app")
    parser.add_argument("-b", "--blid", default="31C7C41492137830", help="Roomba BLID (from SoftAP SSID: Roomba-<BLID>)")
    parser.add_argument("-s", "--ssid", help="Target Wi-Fi SSID (2.4 GHz)")
    parser.add_argument("-p", "--password", help="Target Wi-Fi Password")
    parser.add_argument("-t", "--timezone", default="Europe/Rome", help="Timezone (default: Europe/Rome)")
    parser.add_argument("-c", "--country", default="IT", help="Country code (default: IT)")
    args = parser.parse_args()
    ssid = args.ssid
    if not ssid:
        ssid = input("Enter target 2.4 GHz Wi-Fi SSID: ").strip()
    wifi_psk = args.password
    if not wifi_psk:
        wifi_psk = getpass.getpass("Enter target Wi-Fi Password: ")
    roomba_pwd = generate_roomba_password()
    log.info(f"Generated new Roomba MQTT password: {roomba_pwd}")
    provision_password(roomba_pwd)
    time.sleep(2)
    provision_wifi(args.blid, ssid, wifi_psk, roomba_pwd, args.timezone, args.country)
    print("\n" + "="*55)
    print("SUCCESSFULLY CONFIGURED ROOMBA!")
    print(f"BLID:     {args.blid}")
    print(f"Password: {roomba_pwd}")
    print("="*55 + "\n")
    cred_file = os.path.join(SCRIPT_DIR, "CREDENTIALS.txt")
    with open(cred_file, "w") as f:
        f.write(f"BLID={args.blid}\nPASSWORD={roomba_pwd}\nWIFI_SSID={ssid}\n")
    print(f"Updated credentials saved to {cred_file}")


if __name__ == "__main__":
    main()
