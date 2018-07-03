EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:switches
LIBS:relays
LIBS:motors
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:particle
LIBS:MixController-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Mix Controller - 110V Version"
Date "2018-07-03"
Rev "B"
Comp "Veenema Design Works"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L PARTICLE_PHOTON U?
U 1 1 5AD855AD
P 4950 3500
F 0 "U?" H 4950 3550 60  0000 C CNN
F 1 "PARTICLE_PHOTON" H 4950 2750 60  0000 C CNN
F 2 "" H 5450 3700 60  0000 C CNN
F 3 "" H 5450 3700 60  0000 C CNN
	1    4950 3500
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR?
U 1 1 5AD856E8
P 4050 2500
F 0 "#PWR?" H 4050 2350 50  0001 C CNN
F 1 "+5V" H 4050 2640 50  0000 C CNN
F 2 "" H 4050 2500 50  0001 C CNN
F 3 "" H 4050 2500 50  0001 C CNN
	1    4050 2500
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 5AD8570C
P 4050 3250
F 0 "#PWR?" H 4050 3000 50  0001 C CNN
F 1 "GND" H 4050 3100 50  0000 C CNN
F 2 "" H 4050 3250 50  0001 C CNN
F 3 "" H 4050 3250 50  0001 C CNN
	1    4050 3250
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x08_Male J?
U 1 1 5AD8574E
P 8650 4850
F 0 "J?" H 8650 5250 50  0000 C CNN
F 1 "JST-SM8-Female" V 8550 4800 50  0000 C CNN
F 2 "" H 8650 4850 50  0001 C CNN
F 3 "" H 8650 4850 50  0001 C CNN
	1    8650 4850
	-1   0    0    1   
$EndComp
$Comp
L +5V #PWR?
U 1 1 5AD857DB
P 8350 4350
F 0 "#PWR?" H 8350 4200 50  0001 C CNN
F 1 "+5V" H 8350 4490 50  0000 C CNN
F 2 "" H 8350 4350 50  0001 C CNN
F 3 "" H 8350 4350 50  0001 C CNN
	1    8350 4350
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x08_Male J?
U 1 1 5AD8584C
P 9150 4750
F 0 "J?" H 9150 5150 50  0000 C CNN
F 1 "JST-SM8-Male" V 9100 4700 50  0000 C CNN
F 2 "" H 9150 4750 50  0001 C CNN
F 3 "" H 9150 4750 50  0001 C CNN
	1    9150 4750
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x08_Male J?
U 1 1 5AD85A40
P 8650 2550
F 0 "J?" H 8650 2950 50  0000 C CNN
F 1 "JST-SM8-Female" V 8550 2500 50  0000 C CNN
F 2 "" H 8650 2550 50  0001 C CNN
F 3 "" H 8650 2550 50  0001 C CNN
	1    8650 2550
	-1   0    0    1   
$EndComp
$Comp
L Conn_01x08_Male J?
U 1 1 5AD85A4A
P 9150 2450
F 0 "J?" H 9150 2850 50  0000 C CNN
F 1 "JST-SM8-Male" V 9100 2400 50  0000 C CNN
F 2 "" H 9150 2450 50  0001 C CNN
F 3 "" H 9150 2450 50  0001 C CNN
	1    9150 2450
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR?
U 1 1 5AD85A85
P 8300 2000
F 0 "#PWR?" H 8300 1850 50  0001 C CNN
F 1 "+5V" H 8300 2140 50  0000 C CNN
F 2 "" H 8300 2000 50  0001 C CNN
F 3 "" H 8300 2000 50  0001 C CNN
	1    8300 2000
	1    0    0    -1  
$EndComp
Text Label 9400 4850 0    60   ~ 0
red
Text Label 9400 4950 0    60   ~ 0
yellow
Text Label 9400 5050 0    60   ~ 0
brown
Text Label 9400 5150 0    60   ~ 0
orange
Text Label 9400 2350 0    60   ~ 0
white
Text Label 9400 2150 0    60   ~ 0
green
Text Label 9400 2250 0    60   ~ 0
black
Text Label 9400 2450 0    60   ~ 0
blue
Text Label 9400 2550 0    60   ~ 0
red
Text Label 9400 2650 0    60   ~ 0
yellow
Text Label 9400 2750 0    60   ~ 0
brown
Text Label 9400 2850 0    60   ~ 0
orange
$Comp
L +3V3 #PWR?
U 1 1 5AD86030
P 5700 2500
F 0 "#PWR?" H 5700 2350 50  0001 C CNN
F 1 "+3V3" H 5700 2640 50  0000 C CNN
F 2 "" H 5700 2500 50  0001 C CNN
F 3 "" H 5700 2500 50  0001 C CNN
	1    5700 2500
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 5AD8609C
P 6050 2750
F 0 "#PWR?" H 6050 2500 50  0001 C CNN
F 1 "GND" H 6050 2600 50  0000 C CNN
F 2 "" H 6050 2750 50  0001 C CNN
F 3 "" H 6050 2750 50  0001 C CNN
	1    6050 2750
	1    0    0    -1  
$EndComp
$Comp
L LED D?
U 1 1 5AD86215
P 2900 3950
F 0 "D?" H 2900 4050 50  0000 C CNN
F 1 "LED" H 2900 3850 50  0000 C CNN
F 2 "" H 2900 3950 50  0001 C CNN
F 3 "" H 2900 3950 50  0001 C CNN
	1    2900 3950
	1    0    0    -1  
$EndComp
$Comp
L R R?
U 1 1 5AD862D6
P 3500 3950
F 0 "R?" V 3580 3950 50  0000 C CNN
F 1 "330" V 3500 3950 50  0000 C CNN
F 2 "" V 3430 3950 50  0001 C CNN
F 3 "" H 3500 3950 50  0001 C CNN
	1    3500 3950
	0    1    1    0   
$EndComp
$Comp
L GND #PWR?
U 1 1 5AD86489
P 2500 4100
F 0 "#PWR?" H 2500 3850 50  0001 C CNN
F 1 "GND" H 2500 3950 50  0000 C CNN
F 2 "" H 2500 4100 50  0001 C CNN
F 3 "" H 2500 4100 50  0001 C CNN
	1    2500 4100
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x02_Male J?
U 1 1 5AD864E7
P 3500 5100
F 0 "J?" H 3500 5200 50  0000 C CNN
F 1 "JST-SM2-FEMALE" V 3400 5050 50  0000 C CNN
F 2 "" H 3500 5100 50  0001 C CNN
F 3 "" H 3500 5100 50  0001 C CNN
	1    3500 5100
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x02_Male J?
U 1 1 5AD8666A
P 3050 5200
F 0 "J?" H 3050 5300 50  0000 C CNN
F 1 "JST-SM2-MALE" V 2950 5150 50  0000 C CNN
F 2 "" H 3050 5200 50  0001 C CNN
F 3 "" H 3050 5200 50  0001 C CNN
	1    3050 5200
	-1   0    0    1   
$EndComp
$Comp
L +3V3 #PWR?
U 1 1 5AD86963
P 4650 4850
F 0 "#PWR?" H 4650 4700 50  0001 C CNN
F 1 "+3V3" H 4650 4990 50  0000 C CNN
F 2 "" H 4650 4850 50  0001 C CNN
F 3 "" H 4650 4850 50  0001 C CNN
	1    4650 4850
	1    0    0    -1  
$EndComp
$Comp
L R R?
U 1 1 5AD869E3
P 4300 4600
F 0 "R?" V 4380 4600 50  0000 C CNN
F 1 "10K" V 4300 4600 50  0000 C CNN
F 2 "" V 4230 4600 50  0001 C CNN
F 3 "" H 4300 4600 50  0001 C CNN
	1    4300 4600
	-1   0    0    1   
$EndComp
$Comp
L GND #PWR?
U 1 1 5AD86AA5
P 4300 4850
F 0 "#PWR?" H 4300 4600 50  0001 C CNN
F 1 "GND" H 4300 4700 50  0000 C CNN
F 2 "" H 4300 4850 50  0001 C CNN
F 3 "" H 4300 4850 50  0001 C CNN
	1    4300 4850
	1    0    0    -1  
$EndComp
$Comp
L SW_Push SW?
U 1 1 5AD86C2C
P 2100 5100
F 0 "SW?" H 2150 5200 50  0000 L CNN
F 1 "SW_Push" H 2100 5040 50  0001 C CNN
F 2 "" H 2100 5300 50  0001 C CNN
F 3 "" H 2100 5300 50  0001 C CNN
	1    2100 5100
	1    0    0    -1  
$EndComp
Text Label 6250 3100 0    60   ~ 0
black
Text Label 6250 3300 0    60   ~ 0
green
Text Label 6250 4850 0    60   ~ 0
black
Text Label 6200 4250 0    60   ~ 0
red
Text Label 6250 5050 0    60   ~ 0
green
Text Label 6250 5150 0    60   ~ 0
blue_or_white
Text Notes 6050 6400 0    60   ~ 0
NOTES:\nButton uses 3.3V source while motors use 5V source\nWire colors on motor side are best guess based on photos\nYou may want to use a 9 or 10 pin connector for Motor B to prevent them from being plugged incorrectly\nJST SM connectors picked for panel mount, swap with any connector available
Text Notes 9800 2500 0    60   ~ 0
To Motor B
Text Notes 9800 4800 0    60   ~ 0
To Motor A
Text Label 9400 4650 0    60   ~ 0
white
Text Label 9400 4450 0    60   ~ 0
green
Text Label 9400 4550 0    60   ~ 0
black
Text Label 9400 4750 0    60   ~ 0
blue
Text Notes 9900 4900 0    60   ~ 0
"RED"
Text Label 6250 3400 0    60   ~ 0
blue_or_white
Text Label 6250 3900 0    60   ~ 0
red
Wire Wire Line
	4300 2950 4050 2950
Wire Wire Line
	4050 2950 4050 2500
Wire Wire Line
	4300 3050 4050 3050
Wire Wire Line
	4050 3050 4050 3250
Wire Wire Line
	8450 4450 8350 4450
Wire Wire Line
	8350 4350 8350 4750
Wire Wire Line
	8350 4550 8450 4550
Connection ~ 8350 4450
Wire Wire Line
	8350 4650 8450 4650
Connection ~ 8350 4550
Wire Wire Line
	8350 4750 8450 4750
Connection ~ 8350 4650
Wire Notes Line
	9300 5300 9300 4300
Wire Notes Line
	9300 4300 8500 4300
Wire Notes Line
	8500 4300 8500 5300
Wire Notes Line
	8500 5300 9300 5300
Wire Wire Line
	5600 4050 5850 4050
Wire Wire Line
	5850 4050 5850 5150
Wire Wire Line
	5850 5150 8450 5150
Wire Wire Line
	5600 3950 5950 3950
Wire Wire Line
	5950 3950 5950 5050
Wire Wire Line
	5950 5050 8450 5050
Wire Wire Line
	5600 3850 6050 3850
Wire Wire Line
	5600 3750 6150 3750
Wire Wire Line
	6150 3750 6150 4850
Wire Wire Line
	6150 4850 8450 4850
Wire Wire Line
	8450 2150 8300 2150
Wire Wire Line
	8300 2250 8450 2250
Wire Wire Line
	8300 2350 8450 2350
Wire Wire Line
	8300 2450 8450 2450
Wire Notes Line
	9300 3000 9300 2000
Wire Notes Line
	9300 2000 8500 2000
Wire Notes Line
	8500 2000 8500 3000
Wire Notes Line
	8500 3000 9300 3000
Wire Wire Line
	8300 2000 8300 2450
Connection ~ 8300 2150
Connection ~ 8300 2250
Connection ~ 8300 2350
Wire Wire Line
	5600 3650 6150 3650
Wire Wire Line
	6150 3650 6150 3400
Wire Wire Line
	6050 3300 6050 3550
Wire Wire Line
	6050 3550 5600 3550
Wire Wire Line
	5600 3450 6250 3450
Wire Wire Line
	5850 3100 5850 3350
Wire Wire Line
	5850 3350 5600 3350
Wire Wire Line
	9350 5150 9700 5150
Wire Wire Line
	9350 5050 9700 5050
Wire Wire Line
	9350 4950 9700 4950
Wire Wire Line
	9350 4850 9700 4850
Wire Wire Line
	9350 2850 9700 2850
Wire Wire Line
	9350 2750 9700 2750
Wire Wire Line
	9350 2650 9700 2650
Wire Wire Line
	9350 2550 9700 2550
Wire Wire Line
	9350 2450 9700 2450
Wire Wire Line
	9350 2350 9700 2350
Wire Wire Line
	9350 2250 9700 2250
Wire Wire Line
	9350 2150 9700 2150
Wire Wire Line
	5700 2500 5700 2950
Wire Wire Line
	5700 2950 5600 2950
Wire Wire Line
	5600 3250 5750 3250
Wire Wire Line
	5750 3250 5750 2650
Wire Wire Line
	5750 2650 6050 2650
Wire Wire Line
	6050 2650 6050 2750
Wire Wire Line
	4300 3950 3650 3950
Wire Wire Line
	3350 3950 3050 3950
Wire Wire Line
	2750 3950 2500 3950
Wire Wire Line
	2500 3950 2500 4100
Wire Wire Line
	4300 4050 3950 4050
Wire Wire Line
	3950 4050 3950 5100
Wire Wire Line
	3950 5100 3700 5100
Wire Wire Line
	3700 5200 4650 5200
Wire Wire Line
	4650 5200 4650 4850
Wire Wire Line
	4300 4450 4300 4350
Wire Wire Line
	4300 4350 3950 4350
Connection ~ 3950 4350
Wire Wire Line
	4300 4850 4300 4750
Wire Wire Line
	2850 5100 2300 5100
Wire Wire Line
	1900 5100 1700 5100
Wire Wire Line
	1700 5100 1700 5200
Wire Wire Line
	1700 5200 2850 5200
Wire Notes Line
	3650 4800 3650 5500
Wire Notes Line
	3650 4800 2900 4800
Wire Notes Line
	2900 4800 2900 5500
Wire Notes Line
	2900 5500 3650 5500
Wire Wire Line
	9350 4750 9700 4750
Wire Wire Line
	9350 4650 9700 4650
Wire Wire Line
	9350 4550 9700 4550
Wire Wire Line
	9350 4450 9700 4450
$Comp
L Conn_01x04 J?
U 1 1 5B3B54EE
P 6850 4150
F 0 "J?" H 6850 4350 50  0001 C CNN
F 1 "Conn_01x04" H 6850 3850 50  0001 C CNN
F 2 "" H 6850 4150 50  0001 C CNN
F 3 "" H 6850 4150 50  0001 C CNN
	1    6850 4150
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x04 J?
U 1 1 5B3B5743
P 7700 4250
F 0 "J?" H 7700 4450 50  0001 C CNN
F 1 "Conn_01x04" H 7700 3950 50  0001 C CNN
F 2 "" H 7700 4250 50  0001 C CNN
F 3 "" H 7700 4250 50  0001 C CNN
	1    7700 4250
	-1   0    0    1   
$EndComp
Text Notes 6950 4100 0    59   ~ 0
GND
Text Notes 6950 4200 0    59   ~ 0
SDA1
Text Notes 6950 4300 0    59   ~ 0
SCL1
Text Notes 6950 4400 0    59   ~ 0
VREF1
Text Notes 7600 4100 2    59   ~ 0
NC
Text Notes 7600 4200 2    59   ~ 0
SDA2
Text Notes 7600 4300 2    59   ~ 0
SCL2
Text Notes 7600 4400 2    59   ~ 0
VREF2
Text Notes 6950 3750 0    59   ~ 0
Level Translator
Text Notes 7150 3850 0    39   ~ 0
PCA9306
Text Notes 6950 3900 0    20   ~ 0
https://www.sparkfun.com/products/11955
Wire Notes Line
	7850 4500 7850 3650
Wire Notes Line
	7850 3650 6700 3650
Wire Notes Line
	6700 3650 6700 4500
Wire Notes Line
	6700 4500 7850 4500
Wire Wire Line
	5850 3100 6550 3100
Wire Wire Line
	6550 3100 6550 2550
Wire Wire Line
	6550 2550 8450 2550
Wire Wire Line
	6050 3300 6750 3300
Wire Wire Line
	6750 3300 6750 2750
Wire Wire Line
	6750 2750 8450 2750
Wire Wire Line
	6150 3400 6850 3400
Wire Wire Line
	6850 3400 6850 2850
Wire Wire Line
	6850 2850 8450 2850
Wire Wire Line
	6050 3850 6050 4250
Wire Wire Line
	6050 4250 6650 4250
Wire Wire Line
	6250 3450 6250 4150
Wire Wire Line
	6250 4150 6650 4150
Wire Wire Line
	8050 4150 7900 4150
Wire Wire Line
	8050 2650 8050 4150
Wire Wire Line
	8050 2650 8450 2650
Wire Wire Line
	7900 4250 8050 4250
Wire Wire Line
	8050 4250 8050 4950
Wire Wire Line
	8050 4950 8450 4950
Wire Wire Line
	6650 4350 6550 4350
Wire Wire Line
	6550 4350 6550 3850
$Comp
L +3V3 #PWR?
U 1 1 5B3B70CA
P 6550 3850
F 0 "#PWR?" H 6550 3700 50  0001 C CNN
F 1 "+3V3" H 6550 3990 50  0000 C CNN
F 2 "" H 6550 3850 50  0001 C CNN
F 3 "" H 6550 3850 50  0001 C CNN
	1    6550 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	6650 4050 6450 4050
Wire Wire Line
	6450 4050 6450 4550
$Comp
L GND #PWR?
U 1 1 5B3B725B
P 6450 4550
F 0 "#PWR?" H 6450 4300 50  0001 C CNN
F 1 "GND" H 6450 4400 50  0000 C CNN
F 2 "" H 6450 4550 50  0001 C CNN
F 3 "" H 6450 4550 50  0001 C CNN
	1    6450 4550
	1    0    0    -1  
$EndComp
Wire Wire Line
	7900 4350 8200 4350
Wire Wire Line
	8200 4350 8200 4100
$Comp
L +5V #PWR?
U 1 1 5B3B7411
P 8200 4100
F 0 "#PWR?" H 8200 3950 50  0001 C CNN
F 1 "+5V" H 8200 4240 50  0000 C CNN
F 2 "" H 8200 4100 50  0001 C CNN
F 3 "" H 8200 4100 50  0001 C CNN
	1    8200 4100
	1    0    0    -1  
$EndComp
Text Label 8050 3750 1    60   ~ 0
red
Text Label 8050 4550 3    60   ~ 0
red
$Comp
L Conn_01x02 J?
U 1 1 5B3B474F
P 1800 1450
F 0 "J?" H 1800 1550 50  0000 C CNN
F 1 "Conn_01x02" H 1800 1250 50  0001 C CNN
F 2 "" H 1800 1450 50  0001 C CNN
F 3 "" H 1800 1450 50  0001 C CNN
	1    1800 1450
	1    0    0    -1  
$EndComp
Text Notes 1900 1500 0    60   ~ 0
LINE
Text Notes 1900 1600 0    60   ~ 0
NEUT
$Comp
L Conn_01x02 J?
U 1 1 5B3B484D
P 2550 1450
F 0 "J?" H 2550 1550 50  0000 C CNN
F 1 "Conn_01x02" H 2550 1250 50  0001 C CNN
F 2 "" H 2550 1450 50  0001 C CNN
F 3 "" H 2550 1450 50  0001 C CNN
	1    2550 1450
	-1   0    0    -1  
$EndComp
Text Notes 2450 1500 2    60   ~ 0
+5V
Text Notes 2450 1600 2    60   ~ 0
GND
Wire Notes Line
	1650 1100 1650 1700
Wire Notes Line
	1650 1700 2700 1700
Wire Notes Line
	2700 1700 2700 1100
Wire Notes Line
	2700 1100 1650 1100
Text Notes 1700 1200 0    60   ~ 0
AC/DC  CONVERTER
Wire Wire Line
	1600 1450 1050 1450
Wire Wire Line
	1600 1550 1050 1550
Text Label 1050 1450 0    60   ~ 0
LINE
Text Label 1050 1550 0    60   ~ 0
NEUT
$Comp
L +5V #PWR?
U 1 1 5B3B4B4A
P 2950 1100
F 0 "#PWR?" H 2950 950 50  0001 C CNN
F 1 "+5V" H 2950 1240 50  0000 C CNN
F 2 "" H 2950 1100 50  0001 C CNN
F 3 "" H 2950 1100 50  0001 C CNN
	1    2950 1100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 5B3B4B7C
P 2950 1900
F 0 "#PWR?" H 2950 1650 50  0001 C CNN
F 1 "GND" H 2950 1750 50  0000 C CNN
F 2 "" H 2950 1900 50  0001 C CNN
F 3 "" H 2950 1900 50  0001 C CNN
	1    2950 1900
	1    0    0    -1  
$EndComp
Wire Wire Line
	2750 1450 2950 1450
Wire Wire Line
	2950 1450 2950 1100
Wire Wire Line
	2750 1550 2950 1550
Wire Wire Line
	2950 1550 2950 1900
$EndSCHEMATC
