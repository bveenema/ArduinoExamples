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
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L PARTICLE_PHOTON U?
U 1 1 5AD855AD
P 5800 4150
F 0 "U?" H 5800 4200 60  0000 C CNN
F 1 "PARTICLE_PHOTON" H 5800 3400 60  0000 C CNN
F 2 "" H 6300 4350 60  0000 C CNN
F 3 "" H 6300 4350 60  0000 C CNN
	1    5800 4150
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 3600 4900 3600
Wire Wire Line
	4900 3600 4900 3150
$Comp
L +5V #PWR?
U 1 1 5AD856E8
P 4900 3150
F 0 "#PWR?" H 4900 3000 50  0001 C CNN
F 1 "+5V" H 4900 3290 50  0000 C CNN
F 2 "" H 4900 3150 50  0001 C CNN
F 3 "" H 4900 3150 50  0001 C CNN
	1    4900 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 3700 4900 3700
Wire Wire Line
	4900 3700 4900 3900
$Comp
L GND #PWR?
U 1 1 5AD8570C
P 4900 3900
F 0 "#PWR?" H 4900 3650 50  0001 C CNN
F 1 "GND" H 4900 3750 50  0000 C CNN
F 2 "" H 4900 3900 50  0001 C CNN
F 3 "" H 4900 3900 50  0001 C CNN
	1    4900 3900
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x08_Male J?
U 1 1 5AD8574E
P 8200 5000
F 0 "J?" H 8200 5400 50  0000 C CNN
F 1 "JST-SM8-Female" V 8100 4950 50  0000 C CNN
F 2 "" H 8200 5000 50  0001 C CNN
F 3 "" H 8200 5000 50  0001 C CNN
	1    8200 5000
	-1   0    0    1   
$EndComp
$Comp
L +5V #PWR?
U 1 1 5AD857DB
P 7650 4500
F 0 "#PWR?" H 7650 4350 50  0001 C CNN
F 1 "+5V" H 7650 4640 50  0000 C CNN
F 2 "" H 7650 4500 50  0001 C CNN
F 3 "" H 7650 4500 50  0001 C CNN
	1    7650 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	8000 4600 7650 4600
Wire Wire Line
	7650 4500 7650 4900
Wire Wire Line
	7650 4700 8000 4700
Connection ~ 7650 4600
Wire Wire Line
	7650 4800 8000 4800
Connection ~ 7650 4700
Wire Wire Line
	7650 4900 8000 4900
Connection ~ 7650 4800
$Comp
L Conn_01x08_Male J?
U 1 1 5AD8584C
P 8700 4900
F 0 "J?" H 8700 5300 50  0000 C CNN
F 1 "JST-SM8-Male" V 8650 4850 50  0000 C CNN
F 2 "" H 8700 4900 50  0001 C CNN
F 3 "" H 8700 4900 50  0001 C CNN
	1    8700 4900
	1    0    0    -1  
$EndComp
Wire Notes Line
	8850 5450 8850 4450
Wire Notes Line
	8850 4450 8050 4450
Wire Notes Line
	8050 4450 8050 5450
Wire Notes Line
	8050 5450 8850 5450
Wire Wire Line
	6450 4700 6700 4700
Wire Wire Line
	6700 4700 6700 5300
Wire Wire Line
	6700 5300 8000 5300
Wire Wire Line
	6450 4600 6800 4600
Wire Wire Line
	6800 4600 6800 5200
Wire Wire Line
	6800 5200 8000 5200
Wire Wire Line
	6450 4500 6900 4500
Wire Wire Line
	6900 4500 6900 5100
Wire Wire Line
	6900 5100 8000 5100
Wire Wire Line
	6450 4400 7000 4400
Wire Wire Line
	7000 4400 7000 5000
Wire Wire Line
	7000 5000 8000 5000
$Comp
L Conn_01x08_Male J?
U 1 1 5AD85A40
P 8200 3750
F 0 "J?" H 8200 4150 50  0000 C CNN
F 1 "JST-SM8-Female" V 8100 3700 50  0000 C CNN
F 2 "" H 8200 3750 50  0001 C CNN
F 3 "" H 8200 3750 50  0001 C CNN
	1    8200 3750
	-1   0    0    1   
$EndComp
Wire Wire Line
	8000 3350 7650 3350
Wire Wire Line
	7650 3450 8000 3450
Wire Wire Line
	7650 3550 8000 3550
Wire Wire Line
	7650 3650 8000 3650
$Comp
L Conn_01x08_Male J?
U 1 1 5AD85A4A
P 8700 3650
F 0 "J?" H 8700 4050 50  0000 C CNN
F 1 "JST-SM8-Male" V 8650 3600 50  0000 C CNN
F 2 "" H 8700 3650 50  0001 C CNN
F 3 "" H 8700 3650 50  0001 C CNN
	1    8700 3650
	1    0    0    -1  
$EndComp
Wire Notes Line
	8850 4200 8850 3200
Wire Notes Line
	8850 3200 8050 3200
Wire Notes Line
	8050 3200 8050 4200
Wire Notes Line
	8050 4200 8850 4200
$Comp
L +5V #PWR?
U 1 1 5AD85A85
P 7650 3200
F 0 "#PWR?" H 7650 3050 50  0001 C CNN
F 1 "+5V" H 7650 3340 50  0000 C CNN
F 2 "" H 7650 3200 50  0001 C CNN
F 3 "" H 7650 3200 50  0001 C CNN
	1    7650 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	7650 3200 7650 3650
Connection ~ 7650 3350
Connection ~ 7650 3450
Connection ~ 7650 3550
Wire Wire Line
	6450 4300 7000 4300
Wire Wire Line
	7000 4300 7000 4050
Wire Wire Line
	7000 4050 8000 4050
Wire Wire Line
	8000 3950 6900 3950
Wire Wire Line
	6900 3950 6900 4200
Wire Wire Line
	6900 4200 6450 4200
Wire Wire Line
	6450 4100 6800 4100
Wire Wire Line
	6800 4100 6800 3850
Wire Wire Line
	6800 3850 8000 3850
Wire Wire Line
	8000 3750 6700 3750
Wire Wire Line
	6700 3750 6700 4000
Wire Wire Line
	6700 4000 6450 4000
Wire Wire Line
	8900 5300 9250 5300
Wire Wire Line
	8900 5200 9250 5200
Wire Wire Line
	8900 5100 9250 5100
Wire Wire Line
	8900 5000 9250 5000
Text Label 8950 5000 0    60   ~ 0
red
Text Label 8950 5100 0    60   ~ 0
yellow
Text Label 8950 5200 0    60   ~ 0
brown
Text Label 8950 5300 0    60   ~ 0
orange
Wire Wire Line
	8900 4050 9250 4050
Wire Wire Line
	8900 3950 9250 3950
Wire Wire Line
	8900 3850 9250 3850
Wire Wire Line
	8900 3750 9250 3750
Wire Wire Line
	8900 3650 9250 3650
Wire Wire Line
	8900 3550 9250 3550
Wire Wire Line
	8900 3450 9250 3450
Wire Wire Line
	8900 3350 9250 3350
Text Label 8950 3550 0    60   ~ 0
white
Text Label 8950 3350 0    60   ~ 0
green
Text Label 8950 3450 0    60   ~ 0
black
Text Label 8950 3650 0    60   ~ 0
blue
Text Label 8950 3750 0    60   ~ 0
red
Text Label 8950 3850 0    60   ~ 0
yellow
Text Label 8950 3950 0    60   ~ 0
brown
Text Label 8950 4050 0    60   ~ 0
orange
$Comp
L +3V3 #PWR?
U 1 1 5AD86030
P 6550 3150
F 0 "#PWR?" H 6550 3000 50  0001 C CNN
F 1 "+3V3" H 6550 3290 50  0000 C CNN
F 2 "" H 6550 3150 50  0001 C CNN
F 3 "" H 6550 3150 50  0001 C CNN
	1    6550 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	6550 3150 6550 3600
Wire Wire Line
	6550 3600 6450 3600
$Comp
L GND #PWR?
U 1 1 5AD8609C
P 6900 3400
F 0 "#PWR?" H 6900 3150 50  0001 C CNN
F 1 "GND" H 6900 3250 50  0000 C CNN
F 2 "" H 6900 3400 50  0001 C CNN
F 3 "" H 6900 3400 50  0001 C CNN
	1    6900 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	6450 3900 6600 3900
Wire Wire Line
	6600 3900 6600 3300
Wire Wire Line
	6600 3300 6900 3300
Wire Wire Line
	6900 3300 6900 3400
$Comp
L LED D?
U 1 1 5AD86215
P 3750 4600
F 0 "D?" H 3750 4700 50  0000 C CNN
F 1 "LED" H 3750 4500 50  0000 C CNN
F 2 "" H 3750 4600 50  0001 C CNN
F 3 "" H 3750 4600 50  0001 C CNN
	1    3750 4600
	1    0    0    -1  
$EndComp
$Comp
L R R?
U 1 1 5AD862D6
P 4350 4600
F 0 "R?" V 4430 4600 50  0000 C CNN
F 1 "330" V 4350 4600 50  0000 C CNN
F 2 "" V 4280 4600 50  0001 C CNN
F 3 "" H 4350 4600 50  0001 C CNN
	1    4350 4600
	0    1    1    0   
$EndComp
Wire Wire Line
	5150 4600 4500 4600
Wire Wire Line
	4200 4600 3900 4600
Wire Wire Line
	3600 4600 3350 4600
Wire Wire Line
	3350 4600 3350 4750
$Comp
L GND #PWR?
U 1 1 5AD86489
P 3350 4750
F 0 "#PWR?" H 3350 4500 50  0001 C CNN
F 1 "GND" H 3350 4600 50  0000 C CNN
F 2 "" H 3350 4750 50  0001 C CNN
F 3 "" H 3350 4750 50  0001 C CNN
	1    3350 4750
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x02_Male J?
U 1 1 5AD864E7
P 4350 5750
F 0 "J?" H 4350 5850 50  0000 C CNN
F 1 "JST-SM2-FEMALE" V 4250 5700 50  0000 C CNN
F 2 "" H 4350 5750 50  0001 C CNN
F 3 "" H 4350 5750 50  0001 C CNN
	1    4350 5750
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x02_Male J?
U 1 1 5AD8666A
P 3900 5850
F 0 "J?" H 3900 5950 50  0000 C CNN
F 1 "JST-SM2-MALE" V 3800 5800 50  0000 C CNN
F 2 "" H 3900 5850 50  0001 C CNN
F 3 "" H 3900 5850 50  0001 C CNN
	1    3900 5850
	-1   0    0    1   
$EndComp
Wire Wire Line
	5150 4700 4800 4700
Wire Wire Line
	4800 4700 4800 5750
Wire Wire Line
	4800 5750 4550 5750
Wire Wire Line
	4550 5850 5500 5850
Wire Wire Line
	5500 5850 5500 5500
$Comp
L +3V3 #PWR?
U 1 1 5AD86963
P 5500 5500
F 0 "#PWR?" H 5500 5350 50  0001 C CNN
F 1 "+3V3" H 5500 5640 50  0000 C CNN
F 2 "" H 5500 5500 50  0001 C CNN
F 3 "" H 5500 5500 50  0001 C CNN
	1    5500 5500
	1    0    0    -1  
$EndComp
$Comp
L R R?
U 1 1 5AD869E3
P 5150 5250
F 0 "R?" V 5230 5250 50  0000 C CNN
F 1 "10K" V 5150 5250 50  0000 C CNN
F 2 "" V 5080 5250 50  0001 C CNN
F 3 "" H 5150 5250 50  0001 C CNN
	1    5150 5250
	-1   0    0    1   
$EndComp
Wire Wire Line
	5150 5100 5150 5000
Wire Wire Line
	5150 5000 4800 5000
Connection ~ 4800 5000
$Comp
L GND #PWR?
U 1 1 5AD86AA5
P 5150 5500
F 0 "#PWR?" H 5150 5250 50  0001 C CNN
F 1 "GND" H 5150 5350 50  0000 C CNN
F 2 "" H 5150 5500 50  0001 C CNN
F 3 "" H 5150 5500 50  0001 C CNN
	1    5150 5500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 5500 5150 5400
$Comp
L SW_Push SW?
U 1 1 5AD86C2C
P 2950 5750
F 0 "SW?" H 3000 5850 50  0000 L CNN
F 1 "SW_Push" H 2950 5690 50  0001 C CNN
F 2 "" H 2950 5950 50  0001 C CNN
F 3 "" H 2950 5950 50  0001 C CNN
	1    2950 5750
	1    0    0    -1  
$EndComp
Wire Wire Line
	3700 5750 3150 5750
Wire Wire Line
	2750 5750 2550 5750
Wire Wire Line
	2550 5750 2550 5850
Wire Wire Line
	2550 5850 3700 5850
Wire Notes Line
	4500 5450 4500 6150
Wire Notes Line
	4500 5450 3750 5450
Wire Notes Line
	3750 5450 3750 6150
Wire Notes Line
	3750 6150 4500 6150
Text Label 7100 3750 0    60   ~ 0
black
Text Label 7100 3950 0    60   ~ 0
green
Text Label 7150 5000 0    60   ~ 0
black
Text Label 7150 5100 0    60   ~ 0
yellow_or_red
Text Label 7150 5200 0    60   ~ 0
green
Text Label 7150 5300 0    60   ~ 0
blue_or_white
Text Notes 800  1200 0    60   ~ 0
NOTES:\nButton uses 3.3V source while motors use 5V source\nWire colors on motor side are best guess based on photos\nYou may want to use a 9 or 10 pin connector for Motor B to prevent them from being plugged incorrectly\nJST SM connectors picked for panel mount, swap with any connector available
Text Notes 9500 3700 0    60   ~ 0
To Motor B
Text Notes 9500 4950 0    60   ~ 0
To Motor A
$Comp
L Conn_01x02 J?
U 1 1 5AE82FD7
P 2150 2350
F 0 "J?" H 2150 2450 50  0000 C CNN
F 1 "Conn_01x02" H 2150 2150 50  0001 C CNN
F 2 "" H 2150 2350 50  0001 C CNN
F 3 "" H 2150 2350 50  0001 C CNN
	1    2150 2350
	1    0    0    -1  
$EndComp
$Comp
L Conn_01x02 J?
U 1 1 5AE8311E
P 2750 2450
F 0 "J?" H 2750 2550 50  0000 C CNN
F 1 "Conn_01x02" H 2750 2250 50  0001 C CNN
F 2 "" H 2750 2450 50  0001 C CNN
F 3 "" H 2750 2450 50  0001 C CNN
	1    2750 2450
	-1   0    0    1   
$EndComp
Text Notes 2200 2400 0    60   ~ 0
LINE
Text Notes 2200 2500 0    60   ~ 0
NEUT
Text Notes 2700 2400 2    60   ~ 0
+5V
Text Notes 2700 2500 2    60   ~ 0
GND
Wire Notes Line
	2000 2150 2000 2600
Wire Notes Line
	2000 2600 2900 2600
Wire Notes Line
	2900 2600 2900 2150
Wire Notes Line
	2900 2150 2000 2150
Text Notes 2100 2100 0    60   ~ 0
AC/DC Converter
Wire Wire Line
	1950 2350 1550 2350
Wire Wire Line
	1950 2450 1550 2450
Wire Wire Line
	2950 2350 3150 2350
Wire Wire Line
	3150 2350 3150 2150
Wire Wire Line
	2950 2450 3150 2450
Wire Wire Line
	3150 2450 3150 2700
$Comp
L +5V #PWR?
U 1 1 5AE8362F
P 3150 2150
F 0 "#PWR?" H 3150 2000 50  0001 C CNN
F 1 "+5V" H 3150 2290 50  0000 C CNN
F 2 "" H 3150 2150 50  0001 C CNN
F 3 "" H 3150 2150 50  0001 C CNN
	1    3150 2150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 5AE83748
P 3150 2700
F 0 "#PWR?" H 3150 2450 50  0001 C CNN
F 1 "GND" H 3150 2550 50  0000 C CNN
F 2 "" H 3150 2700 50  0001 C CNN
F 3 "" H 3150 2700 50  0001 C CNN
	1    3150 2700
	1    0    0    -1  
$EndComp
Text Label 1550 2350 0    60   ~ 0
LINE
Text Label 1550 2450 0    60   ~ 0
NEUT
Wire Wire Line
	8900 4900 9250 4900
Wire Wire Line
	8900 4800 9250 4800
Wire Wire Line
	8900 4700 9250 4700
Wire Wire Line
	8900 4600 9250 4600
Text Label 8950 4800 0    60   ~ 0
white
Text Label 8950 4600 0    60   ~ 0
green
Text Label 8950 4700 0    60   ~ 0
black
Text Label 8950 4900 0    60   ~ 0
blue
Text Notes 9600 5050 0    60   ~ 0
"RED"
Text Label 7100 4050 0    60   ~ 0
blue_or_white
Text Label 7100 3850 0    60   ~ 0
yellow_or_red
$EndSCHEMATC
