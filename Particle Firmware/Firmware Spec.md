## DAF21 ECM Software Specification ##

This document covers the intended operation of the DAF21 ECM software.

### TABLE OF CONTENTS ###

1. Overview
2. High Level Feature Specifications
	1. Manager
	9. App Interface (Settings)
	2. Status LED
	7. Charger (Air Pressure in Supply Buckets)
	4. Liquid Sensor
	5. Bucket Sensor
	6. Temperature Sensor
	8. Pressure Sensor
3. Feature Implementation Details


### Modules ###

#### Manager ####
The Manager is responsible for the overall state of the ECM.  It controls which state the machine is in according to the below chart.

![](specAssets/DAF21_SW_Flow_Chart.jpg)

[Chart Source](https://drive.google.com/file/d/13nGyr6byP3q-WBsoDsLEFW2Cua5BrlQh/view?usp=sharing "Draw.io link")


**Inputs**
- Hardware Pins (Enable, Step, Directions and Assert for each pump)
- Change State - True/False (button, remote or app toggle)
- Start Flush - True/False (long button/remote press)

**Software Outputs**
- Change State Processed - True/False
- Pump Speed - Number (steps/sec)

**Requires Update() Polling**

----------

#### App Interface (Settings)
The App Interface (settings) feature handles the serial communication between the ECM and the App. The app interface processes incoming serial messages and sends responses if required. If a command required the ECM to update any of the user settings, it will update the in-process settings and the stored settings. These settings are retained on power reset.

The following is a list of all commands.  Theses commands are sent via the app and/or serial monitor.

| Command            | Description                                                                                                | Setting            | Returns                        |
|--------------------|------------------------------------------------------------------------------------------------------------|--------------------|--------------------------------|
| action             | Gets the current state of the Manager                                                                      | N/A                | Number (state)                 |
| autoReverseSteps   | Gets or Sets the number of Steps for Auto Reversing                                                        | autoReverseSteps   | Number (steps)                 |
| cloudStatus        | Gets the current state of the Wifi/Cloud Connection                                                        | N/A                | "Connected" or "Not Available" |
| chargeTimeout      | Gets or Sets the max charging time                                                                         | chargeTimeout      | Number (milli-seconds)         |
| chargeCoolDown     | Gets or Sets the minimum charging off time                                                                 | chargeCoolDown     | Number (milli-seconds)         |
| currentPressure    | Gets the current charge pressure                                                                           | N/A                | Number (milli-inH2O)           |
| error              | Gets the current error, if any                                                                             | N/A                | String "none" or error message |
| eepromVersion      | Gets or Sets the curent version of the EEPROM. Not intended to be used for setting.                        | version            | Number                         |
| firmwareID         | Gets the ID of the firmware                                                                                | N/A                | Number                         |
| flowRate           | Gets or Sets the total rate of mixed Resin and Hardener. 1 unique value per selector.                      | flowRate           | Number (milli-liters)          |
| incrementer        | Gets the value of the debug_incrementer. Useful to see if a particular function is being called.           | N/A                | Number                         |
| keepOpenVolume     | Gets or Sets the total volume of resin and hardener                                                        | keepOpenVolume     | Number (milli-liters)          |
| resinPumpSpeed     | Gets the last calculated resin pump speed                                                                  | N/A                | Number (steps/sec)             |
| hardenerPumpSpeed  | Gets the last calculated hardener pump speed                                                               | N/A                | Number (steps/sec)             |
| maxNoPressure      | Gets or Sets the maximum amount of time the supply buckets, pressure can be out of range before an error.  | maxNoPressure      | Number (milli-seconds)         |
| minChargingTime    | Gets or Sets the minimum amount of time the supply buckets should charge before allowing mixing            | minChargingTime    | Number (milli-seconds)         |
| minPrimes          | Gets or Sets the min number of primes before mixing is allowed                                             | N/A                | Number                         |
| name               | Gets the name of the ECM. Only returns name if connected to cloud                                          | N/A                | String "wait" or ECM-name      |
| numSelectors       | Gets the number of selector positions                                                                      | N/A                | Number                         |
| onPressure         | Gets or Sets the pressure for the charging pump to come on                                                 | onPressure         | Number (milli-inH2O)           |
| offPressure        | Gets or Sets the pressure for the charging pump to turn off                                                | offPressure        | Number (milli-inH2O)           |
| pailThreshold      | Gets or Sets the threshold of when the ouput bucket sensor should detect a bucket                          | N/A                | Number                         |
| pailState          | Gets the state of the output bucket sensor                                                                 | N/A                | Number (True/False)            |
| pailRaw            | Gets the raw sensor value of the output bucket sensor. Useful for setting "pailThreshold"                  | N/A                | Number                         |
| primeVolume        | Gets or Sets the combined volume of resin and hardener that mixed in each prime cycle                      | primeVolume        | Number (milli-liters)          |
| primeState         | Gets whether the ECM is primed or unprimed                                                                 | N/A                | Number (True/False)            |
| ratioResin         | Gets or Sets the resin portion of the ratio. 1 unique value per selector.                                  | ratioResin         | Number (ratio * 100)           |
| ratioHardener      | Gets or Sets the hardener portion of the mix ratio. 1 unique value per selector.                           | ratioHardener      | Number (ratio * 100)           |
| selector           | Gets the current position of the selector switch                                                           | N/A                | Number                         |
| startFlush         | Initiates a flush cycle                                                                                    | N/A                | N/A                            |
| stepsPerMlResin    | Gets or Sets the resin pump steps per ml                                                                   | stepsPerMlResin    | Number (steps)                 |
| stepsPerMlHardener | Gets or Sets the hardener pump steps per ml                                                                | stepsPerMlHardener | Number (steps)                 |
| togglePump         | Simulates a button or remote switch short press                                                            | N/A                | N/A                            |
| version            | Gets the FW version of the ECM                                                                             | N/A                | Number                         |
| volume             | Gets of Sets the mixed volume of epoxy. 1 unique value per selector.                                       |                    |                                |
| wifiStatus         | Gets or Sets the whether the ECM should be connected to Wifi/Cloud. Disconnected by default and on restart | N/A                | Number (True/False)            |

----------

#### Status LED #####
Controls the RGB Status LED.  Can change to any color, fast and slow blink or breathing.

| Heirarchy |          Status          |   Color  |     Action     |
|:---------:|:------------------------:|:--------:|:--------------:|
| 1         | SW Update Ready          | Yellow   | Blink Slow     |
| 2         | Error                    | Red      | Blink Fast     |
| 3         | Resin and Hardener Empty | Blue/Red | Alternate Slow |
| 4         | Resin Empty              | Blue     | ON             |
| 5         | Hardener Empty           | Red      | ON             |
| 6         | NO Bucket                | Green    | Blink Fast     |
| 7         | NOT Primed               | Green    | Blink Slow     |
| 8         | Flushing                 | Green    | Breathe        |
| 9         | Default                  | Green    | ON             |

**Inputs**
- Hardware Pins (Red, Green and Blue)

**Software Outputs**
- None

**Requires Update() Polling**

----------

#### Charger (Air Pressure in Supply Buckets) ####

**Inputs**

**Software Outputs**

**Requires Update() Polling**

----------
 

#### Liquid Presence Sensor #####
Detects the presence of liquid in the resin and hardener lines. Interfaces with the Liquid Presence Sensor. There are 2 liquid presence sensors, 1 for each channel (resin, hardener)

**Inputs**
- Hardware Pin (Liquid Sensor)

**Software Outputs**
- Present - True/False

**Requires Update() Polling**

----------

#### Bucket Sensor ####

The Buckets Sensor determines the presence of the output bucket that contains the mixed bolus.
It reads and optical sensor that returns an analog signal indicating the distance of an object (higher = closer). The sensor uses a detection threshold to determine if a bucket is present. If the current reading is over the detection threshold, then a bucket is in position.

**Inputs**
- Hardware Pin (Bucket Sensor)
- Detection Threshold - Number

**Software Outputs**
- Current Reading - Number
- State - True/False

**Requires Update() Polling**

----------

#### Temperature Sensor #####
Measures the temperature of the resin or hardener. Determines if the temperature is within and acceptable range to allow mixing.

**Inputs**
- Hardware Pin (Temperature Sensor)
- Min Temperature - Number (Celsius)
- Max Temperature - Number (Celsius)

**Software Outputs**
- In Range - True/False

**Requires Update() Polling**

----------