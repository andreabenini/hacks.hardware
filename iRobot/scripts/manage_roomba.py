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

def create_client(ip, blid, password):
    ctx = ssl.create_default_context()
    ctx.check_hostname = False
    ctx.verify_mode = ssl.CERT_NONE
    ctx.set_ciphers("AES128-SHA256:@SECLEVEL=0")
    client = mqtt.Client(client_id=blid, clean_session=False, protocol=mqtt.MQTTv311)
    client.tls_set_context(ctx)
    client.tls_insecure_set(True)
    client.username_pw_set(blid, password)
    return client

def main():
    creds = parse_credentials()
    parser = argparse.ArgumentParser(description="Manage Roomba preferences and commands over MQTT")
    parser.add_argument("-i", "--ip", default=creds.get("IP", "192.168.0.118"), help="Roomba IP")
    parser.add_argument("-b", "--blid", default=creds.get("BLID", "31C7C41492137830"), help="Roomba BLID")
    parser.add_argument("-p", "--password", default=creds.get("PASSWORD", ""), help="Roomba Password")
    # Information
    parser.add_argument("--get", action="store_true", help="Display current preferences and status")
    # Preferences
    parser.add_argument("--passes", choices=["1", "2", "auto"], help="Cleaning passes: 1, 2, or auto")
    parser.add_argument("--edge-clean", choices=["on", "off"], help="Edge cleaning: on or off")
    parser.add_argument("--bin-pause", choices=["on", "off"], help="Pause when bin is full: on or off")
    parser.add_argument("--carpet-boost", choices=["auto", "performance", "eco"], help="Carpet boost mode")
    # Commands
    parser.add_argument("--cmd", choices=["start", "pause", "stop", "dock", "find"], help="Send command to Roomba")
    args = parser.parse_args()
    if not args.password:
        print("Error: Password not found in CREDENTIALS.txt and not provided via -p")
        sys.exit(1)
    state = {}
    connected = False

    def on_connect(c, userdata, flags, rc):
        nonlocal connected
        if rc == 0:
            connected = True
            c.subscribe("shadow/update")
        else:
            print(f"Failed to connect, rc={rc}")

    def on_message(c, userdata, msg):
        nonlocal state
        try:
            data = json.loads(msg.payload.decode("utf-8"))
            rep = data.get("state", {}).get("reported", {})
            state.update(rep)
        except Exception:
            pass

    client = create_client(args.ip, args.blid, args.password)
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(args.ip, 8883, 10)
    client.loop_start()
    start = time.time()
    while time.time() - start < 3 and ("cleanMissionStatus" not in state or "noAutoPasses" not in state):
        time.sleep(0.3)
    delta_state = {}
    if args.passes == "1":
        delta_state.update({"noAutoPasses": True, "twoPass": False})
    elif args.passes == "2":
        delta_state.update({"noAutoPasses": True, "twoPass": True})
    elif args.passes == "auto":
        delta_state.update({"noAutoPasses": False, "twoPass": False})

    if args.edge_clean == "on":
        delta_state["openOnly"] = False
    elif args.edge_clean == "off":
        delta_state["openOnly"] = True

    if args.bin_pause == "on":
        delta_state["binPause"] = True
    elif args.bin_pause == "off":
        delta_state["binPause"] = False

    if args.carpet_boost == "auto":
        delta_state.update({"carpetBoost": True, "vacHigh": False})
    elif args.carpet_boost == "performance":
        delta_state.update({"carpetBoost": False, "vacHigh": True})
    elif args.carpet_boost == "eco":
        delta_state.update({"carpetBoost": False, "vacHigh": False})

    # Apply preference updates
    if delta_state:
        payload = json.dumps({"state": delta_state})
        print(f"Applying preferences to topic 'delta': {payload}")
        client.publish("delta", payload)
        time.sleep(1)

    # Apply direct commands
    if args.cmd:
        cmd_payload = json.dumps({
            "command": args.cmd,
            "time": int(time.time()),
            "initiator": "localApp"
        })
        print(f"Sending command to topic 'cmd': {cmd_payload}")
        client.publish("cmd", cmd_payload)
        time.sleep(1)

    # Show preferences if requested or if no arguments given
    if args.get or (not delta_state and not args.cmd):
        print("\n=== Current Roomba Configuration ===")
        # Passes
        no_auto = state.get("noAutoPasses")
        two_pass = state.get("twoPass")
        if no_auto is False:
            passes_str = "Auto"
        elif two_pass is True:
            passes_str = "2 Passes"
        else:
            passes_str = "1 Pass"
        print(f"Cleaning Passes:   {passes_str}")
        # Edge Clean
        open_only = state.get("openOnly")
        print(f"Edge Clean:        {'Disabled' if open_only is True else 'Enabled'}")
        # Bin Pause
        bin_pause = state.get("binPause")
        print(f"Pause on Bin Full: {'Yes' if bin_pause is True else 'No (Always Finish)'}")
        # Carpet Boost
        if "carpetBoost" in state:
            cb = state.get("carpetBoost")
            vh = state.get("vacHigh")
            if cb:
                boost = "Auto"
            elif vh:
                boost = "Performance"
            else:
                boost = "Eco"
            print(f"Carpet Boost:      {boost}")
        print(f"Battery Level:     {state.get('batPct', 'N/A')}%")
        cms = state.get("cleanMissionStatus", {})
        print(f"Current Phase:     {cms.get('phase', 'N/A')}")
        print(f"Current Cycle:     {cms.get('cycle', 'N/A')}")
    client.loop_stop()
    client.disconnect()


if __name__ == "__main__":
    main()
