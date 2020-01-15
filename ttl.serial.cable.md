# USB to TTL serial cable
This kind of cables are used to connect TTL RS232-like full-duplex asynchronous serial devices
to any Universal Serial Bus (USB) capable host.  
Mostly used on MCUs where the MAX232/3232 is missing and there's no standard RS232 signal but everything
is using TTL logic signals. Most of them are using Prolific PL2303 terminal adapters because they are cheap, reliable
and widely available of FTDI counterparts. I was a big fan of FTDI chipsets but after their mess with the firmware
affair and consequent driver hell I'm avoiding them like the plague.

# Pinout

| Color | Type  |
| ----- | ----- |
| Red   | 5V    |
| Black | GND   |
| White | RXd   |
| Green | TXd   |


**Note:**  
When you need to connect these adapters to a device you must switch TX/RX on the MCU side as usual if you deal with
a data terminal equipment _(DTE)_ and keep them straight with a data circuit equipment _(DCE)_. Most of them acts as DTE
devices so a null modem connection (rx/tx inverted) is required.
