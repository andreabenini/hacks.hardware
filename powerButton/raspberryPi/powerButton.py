#!/usr/bin/python
#
# pylint: disable=import-error
#
import sys
import time
sys.path.append('/storage/.kodi/addons/virtual.rpi-tools/lib')
import RPi.GPIO as GPIO
import subprocess

# Program defines
IO_OUT = 22                  # Detect this to catch raspberry shutdown
IO_IN  = 23                  # Read shutdown request from the switch (3v3 when detected)

# GPIO Setup
GPIO.setwarnings(False)      # Even if it's not needed, GPIO.cleanup() at the end solves it
GPIO.setmode(GPIO.BCM)

try:
    # Turn ON GPIO(output) to signal power state on
    GPIO.setup(IO_OUT, GPIO.OUT)
    GPIO.output(IO_OUT, GPIO.HIGH)

    # Detect shutdown request from switch (on GPIOinput)
    GPIO.setup(IO_IN, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
    GPIO.wait_for_edge(IO_IN, GPIO.RISING)
    subprocess.call(['shutdown', '-h', 'now'], shell=False)
    while True:               # Waiting shutdown right here
        time.sleep(10)

except:
    print("Something wrong")
#finally:
#    GPIO.cleanup()
