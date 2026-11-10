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

DEFAULT_CREDENTIALS = "CREDENTIALS.txt"


def parse_credentials(path):
    creds = {}
    cred_file = os.path.join(SCRIPT_DIR, path)
    if not os.path.exists(cred_file):
        print(f"Error: credentials file not found: {cred_file}")
        sys.exit(1)
    with open(cred_file, "r") as f:
        for line in f:
            line = line.strip()
            if line and not line.startswith("#") and "=" in line:
                k, v = line.split("=", 1)
                creds[k.strip()] = v.strip()
    return creds


def create_client(blid, password):
    ctx = ssl.create_default_context()
    ctx.check_hostname = False
    ctx.verify_mode = ssl.CERT_NONE
    ctx.set_ciphers("AES128-SHA256:@SECLEVEL=0")
    client = mqtt.Client(client_id=blid, clean_session=False, protocol=mqtt.MQTTv311)
    client.tls_set_context(ctx)
    client.tls_insecure_set(True)
    client.username_pw_set(blid, password)
    return client


# Robot reports installed voice languages as [{"en-UK": 0}, {"fr-FR": 1}, ...]
def available_languages(state):
    langs = {}
    for entry in state.get("langs", []):
        if isinstance(entry, dict):
            langs.update(entry)
    return langs


# Map the reported "language" index back to its locale code
def current_language(state):
    index = state.get("language")
    for code, i in available_languages(state).items():
        if i == index:
            return code
    return None if index is None else f"index {index}"


# Accept a full code (de-DE) or just the language part (de), case-insensitive
def resolve_language(requested, langs):
    req = requested.lower()
    exact = [c for c in langs if c.lower() == req]
    if exact:
        return exact[0]
    matches = [c for c in langs if c.lower().split("-")[0] == req]
    if len(matches) == 1:
        return matches[0]
    if matches:
        raise ValueError(f"'{requested}' is ambiguous: {', '.join(matches)}")
    raise ValueError(f"'{requested}' not available. Installed: {', '.join(langs) or 'none reported'}")


# Report available languages
def print_languages(state):
    langs = available_languages(state)
    current = current_language(state)
    print(f"Current Language:  {current or 'N/A'}")
    print("Available:")
    for code, i in sorted(langs.items(), key=lambda kv: kv[1]):
        marker = "*" if code == current else " "
        print(f"  {marker} {code}")


# Print roomba configuration
def print_configuration(state):
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
    print(f"Edge Clean:        {'Disabled' if state.get('openOnly') is True else 'Enabled'}")
    # Bin Pause
    print(f"Pause on Bin Full: {'Yes' if state.get('binPause') is True else 'No (Always Finish)'}")
    # Carpet Boost
    if "carpetBoost" in state:
        if state.get("carpetBoost"):
            boost = "Auto"
        elif state.get("vacHigh"):
            boost = "Performance"
        else:
            boost = "Eco"
        print(f"Carpet Boost:      {boost}")
    print(f"Language:          {current_language(state) or 'N/A'}")
    print(f"Battery Level:     {state.get('batPct', 'N/A')}%")
    cms = state.get("cleanMissionStatus", {})
    print(f"Current Phase:     {cms.get('phase', 'N/A')}")
    print(f"Current Cycle:     {cms.get('cycle', 'N/A')}")


# MAIN #
def main():
    # Read --robot first so the chosen credentials file provides the defaults below
    pre = argparse.ArgumentParser(add_help=False)
    pre.add_argument("--robot", default=DEFAULT_CREDENTIALS)
    known, _ = pre.parse_known_args()
    creds = parse_credentials(known.robot)

    parser = argparse.ArgumentParser(description="Manage Roomba preferences and commands over MQTT")
    parser.add_argument("--robot", default=DEFAULT_CREDENTIALS, help=f"Credentials file (default: {DEFAULT_CREDENTIALS})")
    parser.add_argument("-i", "--ip", default=creds.get("IP", "192.168.0.118"), help="Roomba IP")
    parser.add_argument("-b", "--blid", default=creds.get("BLID", ""), help="Roomba BLID")
    parser.add_argument("-p", "--password", default=creds.get("PASSWORD", ""), help="Roomba Password")
    # Information
    parser.add_argument("--get", action="store_true", help="Display current preferences and status")
    # Preferences
    parser.add_argument("--passes", choices=["1", "2", "auto"], help="Cleaning passes: 1, 2, or auto")
    parser.add_argument("--edge-clean", choices=["on", "off"], help="Edge cleaning: on or off")
    parser.add_argument("--bin-pause", choices=["on", "off"], help="Pause when bin is full: on or off")
    parser.add_argument("--carpet-boost", choices=["auto", "performance", "eco"], help="Carpet boost mode")
    parser.add_argument("--language", nargs="?", const="", metavar="CODE", help="Without a value: list languages. With a value (e.g. de, fr-FR): switch voice language")
    # Commands
    parser.add_argument("--cmd", choices=["start", "pause", "stop", "resume", "dock", "find"], help="Send command to Roomba")
    args = parser.parse_args()
    if not args.blid or not args.password:
        print(f"Error: BLID/PASSWORD not found in {args.robot} and not provided via -b/-p")
        sys.exit(1)
    state = {}
    connack = {}

    def on_connect(c, userdata, flags, rc):
        connack["rc"] = rc
        if rc == 0:
            c.subscribe("shadow/update")

    def on_message(c, userdata, msg):
        try:
            data = json.loads(msg.payload.decode("utf-8"))
            state.update(data.get("state", {}).get("reported", {}))
        except Exception:
            pass

    # State keys to wait for before acting
    required = {"cleanMissionStatus", "noAutoPasses"}
    if args.language is not None:
        required |= {"language", "langs"}
    client = create_client(args.blid, args.password)
    client.on_connect = on_connect
    client.on_message = on_message
    try:
        client.connect(args.ip, 8883, 10)
    # The robot accepts a single local MQTT client: refused usually means Home Assistant holds it
    except ConnectionRefusedError:
        print(f"Error: connection refused by {args.ip}:8883 - is Home Assistant (or another client) connected?")
        sys.exit(1)
    except OSError as e:
        print(f"Error: cannot reach {args.ip}:8883 - {e}")
        sys.exit(1)
    client.loop_start()
    start = time.time()
    while time.time() - start < 5 and not required <= state.keys() and connack.get("rc", 0) == 0:
        time.sleep(0.3)
    # Non-zero CONNACK: broker reachable but credentials rejected
    if connack.get("rc", 0) != 0:
        client.loop_stop()
        print(f"Error: {mqtt.connack_string(connack['rc'])} - check BLID/PASSWORD in {args.robot}")
        sys.exit(1)

    def finish(code=0):
        client.loop_stop()
        client.disconnect()
        sys.exit(code)

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

    # Language: resolve the requested code to the robot's index
    if args.language:
        langs = available_languages(state)
        if not langs:
            print("Error: robot did not report its installed languages")
            finish(1)
        try:
            code = resolve_language(args.language, langs)
        except ValueError as e:
            print(f"Error: {e}")
            finish(1)
        delta_state["language"] = langs[code]
        print(f"Switching language: {current_language(state) or 'N/A'} -> {code}")

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

    # --language without a value: list current and installed languages
    if args.language == "":
        if "langs" not in state:
            print("Error: robot did not report its installed languages")
            finish(1)
        print_languages(state)

    # Show preferences if requested or if no arguments given
    if args.get or (not delta_state and not args.cmd and args.language is None):
        print_configuration(state)

    finish()


if __name__ == "__main__":
    main()
