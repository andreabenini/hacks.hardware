EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "tinySwitch"
Date "2020-12-06"
Rev "1.2"
Comp "Ben"
Comment1 ""
Comment2 "Tiny85 USB with 1 channel relay"
Comment3 ""
Comment4 "ON/OFF switch for Raspberry Pi"
$EndDescr
$Comp
L Relay:DIPxx-1Axx-12xD K1
U 1 1 5FBA1307
P 6000 2350
F 0 "K1" V 6467 2350 50  0000 C CNN
F 1 "1 Channel Relay" V 6376 2350 50  0000 C CNN
F 2 "Relay_THT:Relay_StandexMeder_DIP_LowProfile" H 6350 2300 50  0001 L CNN
F 3 "https://standexelectronics.com/wp-content/uploads/datasheet_reed_relay_DIP.pdf" H 6000 2350 50  0001 C CNN
	1    6000 2350
	0    -1   -1   0   
$EndComp
$Comp
L MCU_Microchip_ATtiny:ATtiny85V-10SU U1
U 1 1 5FB9B9E0
P 7700 2550
F 0 "U1" V 7033 2550 50  0000 C CNN
F 1 "Digispark Tiny85" V 7124 2550 50  0000 C CNN
F 2 "Package_SO:SOIJ-8_5.3x5.3mm_P1.27mm" H 7700 2550 50  0001 C CIN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/atmel-2586-avr-8-bit-microcontroller-attiny25-attiny45-attiny85_datasheet.pdf" H 7700 2550 50  0001 C CNN
	1    7700 2550
	0    -1   1    0   
$EndComp
Wire Wire Line
	6300 2550 6550 2550
Wire Wire Line
	7400 3150 5700 3150
Wire Wire Line
	5700 3150 5700 2550
$Comp
L Switch:SW_Push SW1
U 1 1 5FBAA7A8
P 6750 3650
F 0 "SW1" H 6750 3850 50  0000 C CNN
F 1 "Power Button" H 6750 3550 50  0000 C CNN
F 2 "" H 6750 3850 50  0001 C CNN
F 3 "~" H 6750 3850 50  0001 C CNN
	1    6750 3650
	1    0    0    -1  
$EndComp
Wire Wire Line
	6550 3650 6550 2550
Connection ~ 6550 2550
Wire Wire Line
	6550 2550 7100 2550
Wire Wire Line
	6950 3650 7500 3650
Wire Wire Line
	7500 3650 7500 3150
$Comp
L power:Earth #PWR?
U 1 1 5FBAF213
P 6000 2800
F 0 "#PWR?" H 6000 2550 50  0001 C CNN
F 1 "Earth" H 6000 2650 50  0001 C CNN
F 2 "" H 6000 2800 50  0001 C CNN
F 3 "~" H 6000 2800 50  0001 C CNN
	1    6000 2800
	1    0    0    -1  
$EndComp
$Comp
L power:Earth #PWR?
U 1 1 5FBB0448
P 8700 3750
F 0 "#PWR?" H 8700 3500 50  0001 C CNN
F 1 "Earth" H 8700 3600 50  0001 C CNN
F 2 "" H 8700 3750 50  0001 C CNN
F 3 "~" H 8700 3750 50  0001 C CNN
	1    8700 3750
	1    0    0    -1  
$EndComp
$Comp
L Device:R R4
U 1 1 5FBB20BE
P 8200 3650
F 0 "R4" V 8100 3650 50  0000 C CNN
F 1 "10K" V 8200 3650 50  0000 C CNN
F 2 "" V 8130 3650 50  0001 C CNN
F 3 "~" H 8200 3650 50  0001 C CNN
	1    8200 3650
	0    1    1    0   
$EndComp
Wire Wire Line
	7500 3650 8050 3650
Connection ~ 7500 3650
Wire Wire Line
	8350 3650 8700 3650
Wire Wire Line
	8700 3650 8700 2550
Wire Wire Line
	8700 2550 8450 2550
Connection ~ 8700 3650
Wire Wire Line
	6550 2550 6550 2150
Wire Wire Line
	6550 2150 6300 2150
Wire Wire Line
	6550 2150 6550 1650
Wire Wire Line
	6550 1650 6050 1650
Wire Wire Line
	6050 1650 6050 1400
Connection ~ 6550 2150
Wire Wire Line
	5700 2150 5600 2150
Wire Wire Line
	5600 2150 5600 1650
Wire Wire Line
	5600 1650 5950 1650
Wire Wire Line
	5950 1650 5950 1400
Wire Wire Line
	8700 2550 8700 1550
Wire Wire Line
	8700 1550 6150 1550
Wire Wire Line
	6150 1550 6150 1400
Connection ~ 8700 2550
Wire Wire Line
	7600 3150 7600 3450
Wire Wire Line
	7600 3450 8050 3450
$Comp
L Device:R R2
U 1 1 5FBBB646
P 8200 3250
F 0 "R2" V 8100 3250 50  0000 C CNN
F 1 "2K2" V 8200 3250 50  0000 C CNN
F 2 "" V 8130 3250 50  0001 C CNN
F 3 "~" H 8200 3250 50  0001 C CNN
	1    8200 3250
	0    1    1    0   
$EndComp
$Comp
L Device:R R1
U 1 1 5FBBD94C
P 8450 2950
F 0 "R1" H 8350 2950 50  0000 C CNN
F 1 "3K3" V 8450 2950 50  0000 C CNN
F 2 "" V 8380 2950 50  0001 C CNN
F 3 "~" H 8450 2950 50  0001 C CNN
	1    8450 2950
	-1   0    0    1   
$EndComp
Wire Wire Line
	8450 2800 8450 2550
Connection ~ 8450 2550
Wire Wire Line
	8450 2550 8300 2550
$Comp
L Device:R R3
U 1 1 5FBC97E0
P 8200 3450
F 0 "R3" V 8100 3450 50  0000 C CNN
F 1 "1K" V 8200 3450 50  0000 C CNN
F 2 "" V 8130 3450 50  0001 C CNN
F 3 "~" H 8200 3450 50  0001 C CNN
	1    8200 3450
	0    1    1    0   
$EndComp
Wire Wire Line
	8050 3250 7800 3250
Wire Wire Line
	7800 3250 7800 3150
Text Notes 7650 2600 0    39   ~ 0
 USB\nD+ D-
Wire Wire Line
	8350 3250 8450 3250
Wire Wire Line
	8450 3250 8450 3100
Connection ~ 8450 3250
Wire Wire Line
	8450 3250 9000 3250
Wire Wire Line
	8350 3450 8450 3450
Wire Wire Line
	8450 3450 8450 3350
Wire Wire Line
	8450 3350 9000 3350
Wire Wire Line
	8700 3650 8700 3750
$Comp
L Connector_Generic:Conn_01x02 J2
U 1 1 5FC1178B
P 9200 3250
F 0 "J2" H 9150 3400 50  0000 L CNN
F 1 "Conn_01x02" H 9280 3242 50  0001 L CNN
F 2 "" H 9200 3250 50  0001 C CNN
F 3 "~" H 9200 3250 50  0001 C CNN
F 4 "RPi Shutdown Line (output)  [GPIO22]" H 9300 3250 50  0000 L CNN "Output"
F 5 "RPi Detection Line (input)    [GPIO23]" H 9300 3150 50  0000 L CNN "Input"
	1    9200 3250
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J1
U 1 1 5FC158BD
P 6050 1200
F 0 "J1" V 6250 1250 50  0000 R CNN
F 1 "Conn_01x03" H 6130 1151 50  0001 L CNN
F 2 "" H 6050 1200 50  0001 C CNN
F 3 "~" H 6050 1200 50  0001 C CNN
F 4 "VOut" V 6150 1450 50  0000 R CNN "vout"
F 5 "VIn" V 6150 1250 50  0000 R CNN "vin"
F 6 "GND" V 6150 1100 50  0000 R CNN "gnd"
	1    6050 1200
	0    -1   -1   0   
$EndComp
$Comp
L Connector:Raspberry_Pi_2_3 JPi
U 1 1 5FC39232
P 3500 6050
F 0 "JPi" H 3500 7950 50  0000 C CNN
F 1 "Raspberry Pi" H 3500 7850 50  0000 C CNN
F 2 "" H 3500 6050 50  0001 C CNN
F 3 "https://www.raspberrypi.org/documentation/hardware/raspberrypi/schematics/rpi_SCH_3bplus_1p0_reduced.pdf" H 3500 6050 50  0001 C CNN
	1    3500 6050
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J3
U 1 1 5FC472EA
P 1500 4400
F 0 "J3" V 1500 4500 50  0000 L CNN
F 1 "Conn_01x02" H 1580 4392 50  0001 L CNN
F 2 "" H 1500 4400 50  0001 C CNN
F 3 "~" H 1500 4400 50  0001 C CNN
F 4 "Power Supply from microUSB cable (VOut, GND)" V 1450 1450 50  0000 L CNN "Supply"
F 5 "VOut" V 1650 4250 50  0000 C CNN "Vcc"
F 6 "GND" V 1650 4450 50  0000 C CNN "GND"
	1    1500 4400
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3300 4750 1600 4750
Wire Wire Line
	1600 4750 1600 4600
Wire Wire Line
	3100 7350 1500 7350
Wire Wire Line
	1500 7350 1500 4600
$Comp
L Connector_Generic:Conn_01x02 J4
U 1 1 5FC64BB2
P 2000 4400
F 0 "J4" V 2000 4500 50  0000 L CNN
F 1 "Conn_01x02" H 2080 4392 50  0001 L CNN
F 2 "" H 2000 4400 50  0001 C CNN
F 3 "~" H 2000 4400 50  0001 C CNN
F 4 "Detect" H 2100 4400 50  0000 L CNN "Output"
F 5 "Shutdown" H 2100 4300 50  0000 L CNN "Input"
	1    2000 4400
	0    -1   -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J0
U 1 1 5FC924C4
P 3000 2350
F 0 "J0" V 3000 2450 50  0000 L CNN
F 1 "Conn_01x02" H 3080 2342 50  0001 L CNN
F 2 "" H 3000 2350 50  0001 C CNN
F 3 "~" H 3000 2350 50  0001 C CNN
F 4 "GND" V 3100 2300 50  0000 L CNN "GND"
F 5 "VIn" V 3100 2150 50  0000 L CNN "VIn"
	1    3000 2350
	0    1    1    0   
$EndComp
$Comp
L Connector:Screw_Terminal_01x02 J?
U 1 1 5FC9F5A1
P 2900 1550
F 0 "J?" V 2864 1362 50  0001 R CNN
F 1 "External Power Supply" V 2900 1350 50  0000 R CNN
F 2 "" H 2900 1550 50  0001 C CNN
F 3 "~" H 2900 1550 50  0001 C CNN
	1    2900 1550
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2900 1750 2900 2150
Wire Wire Line
	3000 1750 3000 2150
Text Notes 5700 3250 0    49   ~ 0
Signal to toggle relay
Wire Wire Line
	2000 6350 2700 6350
Wire Wire Line
	2000 4600 2000 6350
Wire Wire Line
	2100 4600 2100 6250
Wire Wire Line
	2100 6250 2700 6250
Wire Bus Line
	3250 2350 4500 2350
Wire Bus Line
	4500 2350 4500 1200
Wire Bus Line
	4500 1200 5750 1200
$EndSCHEMATC
