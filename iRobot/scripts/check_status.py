#!/usr/bin/env python3
import os
import sys

# Ensure OpenSSL legacy renegotiation configuration is loaded
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
CONF_PATH = os.path.join(SCRIPT_DIR, "openssl_legacy.cnf")
if os.path.exists(CONF_PATH) and os.environ.get("OPENSSL_CONF") != CONF_PATH:
    os.environ["OPENSSL_CONF"] = CONF_PATH
    os.execv(sys.executable, [sys.executable] + sys.argv)

import argparse
import json
import ssl
import time
import paho.mqtt.client as mqtt

def parse_credentials():
    creds = {}
    cred_file = os.path.join(SCRIPT_DIR, "CREDENTIALS.txt")
    if os.path.exists(cred_file):
        with open(cred_file, "r") as f:
            for line in f:
                line = line.strip()
                if line and not line.startswith("#") and "=" in line:
                    k, v = line.split("=", 1)
                    creds[k.strip()] = v.strip()
    return creds

def main():
    creds = parse_credentials()
    parser = argparse.ArgumentParser(description="Check Roomba battery and mission status via MQTT")
    parser.add_argument("-i", "--ip", default=creds.get("IP", "192.168.0.222"), help="Roomba IP")
    parser.add_argument("-b", "--blid", default=creds.get("BLID", "1199999999666620"), help="Roomba BLID")
    parser.add_argument("-p", "--password", default=creds.get("PASSWORD", ""), help="Roomba Password")
    args = parser.parse_args()
    if not args.password:
        print("Error: Password not provided and not found in CREDENTIALS.txt")
        sys.exit(1)
    ctx = ssl.create_default_context()
    ctx.check_hostname = False
    ctx.verify_mode = ssl.CERT_NONE
    ctx.set_ciphers("AES128-SHA256:@SECLEVEL=0")
    received = False

    def on_connect(c, userdata, flags, rc):
        if rc == 0:
            c.subscribe("shadow/update")
            c.subscribe("wifictl")
        else:
            print(f"Connection failed with return code {rc}")

    def on_message(c, userdata, msg):
        nonlocal received
        try:
            data = json.loads(msg.payload.decode("utf-8"))
            reported = data.get("state", {}).get("reported", {})
            if "batPct" in reported:
                received = True
                print("\n=== Roomba Status ===")
                print(f"Battery:       {reported.get('batPct')}%")
                bin_full = reported.get('bin', {}).get('full')
                if bin_full is not None:
                    print(f"Bin Full:      {bin_full}")
                cms = reported.get('cleanMissionStatus', {})
                print(f"Phase:         {cms.get('phase', 'unknown')}")
                print(f"Cycle:         {cms.get('cycle', 'unknown')}")
                print(f"Error Code:    {cms.get('error', 0)}")
                c.disconnect()
        except Exception:
            pass

    client = mqtt.Client(client_id=args.blid, clean_session=False, protocol=mqtt.MQTTv311)
    client.tls_set_context(ctx)
    client.tls_insecure_set(True)
    client.username_pw_set(args.blid, args.password)
    client.on_connect = on_connect
    client.on_message = on_message

    print(f"Connecting to Roomba at {args.ip}:8883...")
    client.connect(args.ip, 8883, 10)
    client.loop_start()
    start = time.time()
    while time.time() - start < 5 and not received:
        time.sleep(0.5)
    client.loop_stop()
    if not received:
        print("Did not receive state packet within timeout.")

if __name__ == "__main__":
    main()
