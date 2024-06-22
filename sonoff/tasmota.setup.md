# Tasmota firmware configuration and API setup

## Basic and useful commands
A complete reference on possible commands and API documentation from
https://tasmota.github.io/docs/Commands/
```sh
export IP="http://your.tasmota.device.ip"
export CREDENTIALS="user=admin&password=WhatEver"

## Turn ON/OFF or TOGGLE the connected load
curl "$IP/cm?$CREDENTIALS&cmnd=Power%20ON"
curl "$IP/cm?$CREDENTIALS&cmnd=Power%20OFF"
curl "$IP/cm?$CREDENTIALS&cmnd=Power%20TOGGLE"

## Detect PowerOnState after boot
curl "$IP/cm?$CREDENTIALS&cmnd=PowerOnState"
## Set PowerOnState to 0
curl "$IP/cm?$CREDENTIALS&cmnd=PowerOnState%200"

# PowerOnState to 5, pulsetimer to 30secs
curl -X GET "$IP/cm?$CREDENTIALS&cmnd=PowerOnState%205"
curl -X GET "$IP/cm?$CREDENTIALS&cmnd=PulseTime%20130"
```

## Examples
### Turn controller device **_ON_** exactly 30 seconds after boot
From the Tasmota firmware console in the web interface it's possible to enter commands
directly without URL enconding or passing them on the CLI, way easier to test it as well.  
- Keep device OFF on boot
- Define **rule 1**
- Turn ON **rule 1**, set a 30sec timer and turn the appliance on after that delay
```
PowerOnState 0
rule 1
rule on system#boot do ruletimer1 30 endon on Rules#Timer=1 do power on endon
```

### Configuring device from the CLI
In this example a configuration is sent directly from the cli with `curl`
```sh
# Rule configuration, oneliner. Take some extracare on URL encoding (needed)
curl "$IP/cm?$CREDENTIALS&cmnd=Rule1%20ON%20System#Boot%20DO%20Delay%20300%20ENDON%20ON%20Rules%23Timer%201%20DO%20Power%20On%20ENDON"

# Enable the rule
curl "$IP/cm?$CREDENTIALS&cmnd=Rule1%20ON"

# Verify the rule
curl -X GET "$IP/cm?$CREDENTIALS&cmnd=Rule1"
```
