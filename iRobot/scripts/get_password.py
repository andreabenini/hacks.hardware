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
import socket
import ssl
import time

REQ = bytes.fromhex("f005efcc3b2900")

def discover_roomba():
    print("Sending UDP discovery broadcast on port 5678...")
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    s.settimeout(4)
    s.bind(("", 5678))
    s.sendto(b"irobotmcs", ("255.255.255.255", 5678))
    
    robots = []
    start = time.time()
    while time.time() - start < 4:
        try:
            data, addr = s.recvfrom(2048)
            msg = json.loads(data.decode("utf-8", errors="ignore"))
            if "hostname" in msg and ("Roomba" in msg["hostname"] or "iRobot" in msg["hostname"]):
                robots.append((addr[0], msg))
        except (socket.timeout, Exception):
            pass
    s.close()
    return robots

def fetch_password(ip, timeout=60):
    ctx = ssl.create_default_context()
    ctx.check_hostname = False
    ctx.verify_mode = ssl.CERT_NONE
    ctx.set_ciphers("AES128-SHA256:@SECLEVEL=0")

    print(f"\n[!] Ensure Roomba is on its Home Base and awake.")
    print(f"[!] Press and hold the HOME button on the robot for ~2 seconds until you hear a chime.")
    print(f"Waiting for Roomba response at {ip}:8883...")

    deadline = time.time() + timeout
    while time.time() < deadline:
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(2)
            wrapped = ctx.wrap_socket(sock)
            wrapped.connect((ip, 8883))
            wrapped.sendall(REQ)

            resp = b""
            while len(resp) < 37:
                chunk = wrapped.recv(1024)
                if not chunk:
                    break
                resp += chunk
            wrapped.close()

            if len(resp) > 7 and resp[0] == 0xf0 and resp[6] == 0x00:
                pwd = resp[7:].decode("ascii", errors="ignore")
                if "\x00" in pwd:
                    pwd = pwd[:pwd.find("\x00")]
                return pwd
        except Exception:
            pass
        time.sleep(1)
    return None

def main():
    parser = argparse.ArgumentParser(description="Query Roomba local MQTT password while HOME button is pressed")
    parser.add_argument("-i", "--ip", help="Roomba IP on LAN (auto-discovered if not provided)")
    args = parser.parse_args()

    ip = args.ip
    if not ip:
        robots = discover_roomba()
        if not robots:
            print("No Roombas discovered via UDP broadcast. Please specify IP with -i <IP>.")
            sys.exit(1)
        print(f"Found {len(robots)} Roomba(s):")
        for idx, (rip, info) in enumerate(robots):
            print(f" [{idx}] {info.get('robotname', 'Roomba')} - IP: {rip} - BLID: {info.get('hostname')}")
        ip = robots[0][0]
        print(f"Selecting {ip}")

    pwd = fetch_password(ip)
    if pwd:
        print("\n" + "="*50)
        print(f"SUCCESS! ROOMBA PASSWORD FOUND:")
        print(f"Password: {pwd}")
        print("="*50 + "\n")
        cred_file = os.path.join(SCRIPT_DIR, "CREDENTIALS.txt")
        print(f"Remember to update or check {cred_file}")
    else:
        print("\nTimed out waiting for HOME button pairing signal.")
        sys.exit(1)

if __name__ == "__main__":
    main()
