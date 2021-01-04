# Adapt AT/ATX power supply unit as a generic power supply
It's quite easy to adapt an AT/ATX power supply to work as a generic power supply for your experiments,
you just need to follow these basic steps. You also need to always keep in mind you might have 70-75%
efficiency from it (when you're lucky), sometimes even more but you need to really need to have a top
class device for it.

## Pinout
Here are two common power connectors: 20 and 24 pins
![Power Supply Pinout](PowerSupplyPinout.png)

- The wires on the main 20 pin (or 24 pin) connectors are color coded. These are the same for all 
ATX power supplies, please also consult your power supply functional specs (if any) to know how many
Amps you can get from there. these are color coded cables from it:
    - 3.3V wires are orange
    - +5V wires are red
    - -5V wires (if they are present) are white
    - +12V wires are yellow
    - -12V wires are blue
    - ground wires are black.
- The green wire is the "power on" sensor. This wire is internally connected to 5V with a pull-up
resistor. If you connect this wire to ground (any black wire) the power supply will turn on.
- The purple wire is the +5 "stand by" power. This outputs a 5V signal even if the rest of the power
supply has not yet turned on, allowing you to power any circuit that might control the ON/OFF signal.
- The gray "power good" indicator is at 5V if each of the output wires is operating at the correct
voltages.

## Minimum load requirements
Many power supplies will not stay on unless the output reaches a certain minimum power requirement.
To test if your power supply has this requirement, press the power button or turn it on with the
green wire and wait for a few minutes after that. If it shuts itself off,
then you know that your power supply has a minimum output requirement.

To take care of it, you need to add a power resistor between one 5V terminal and ground.
In most cases, a 10 watt 10 ohm resistor will work for it.
In very rare cases, you may also have minimum output requirements on the 12V pin and the 3.3V pin.
This will require additional power resistors.

These power resistors create a lot of heat, if you add a power resistor make sure that your
project housing has adequate ventilation. In some cases you may even need to add a small PC
fan to help dissipate the heat, it might be a good idea to connect the resistor to the PSU
case or keep it inside the case if there's some room for it
