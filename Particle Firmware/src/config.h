#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

#define THIS_PRODUCT_ID 7951
#define THIS_PRODUCT_VERSION 14

#define RESIN_PUMP_ENABLE_PIN D0
#define RESIN_PUMP_DIR_PIN D1
#define RESIN_PUMP_STEP_PIN D2
#define RESIN_PUMP_ASSERT_PIN D3

#define HARDENER_PUMP_ENABLE_PIN D4
#define HARDENER_PUMP_DIR_PIN D5
#define HARDENER_PUMP_STEP_PIN D6
#define HARDENER_PUMP_ASSERT_PIN D7

#define PRESS_SNS_PIN A0
#define PAIL_SNS_PIN A1
#define AIR_PUMP_EN A2

#define BUTTON_PIN A3
#define STATUS_LED_PIN A4
#define REMOTE_PIN A5

#define SELECTOR_SWITCH_4 DAC
#define SELECTOR_SWITCH_3 WKP
#define SELECTOR_SWITCH_2 RX
#define SELECTOR_SWITCH_1 TX

#define NUM_SELECTORS 5

// Status LED Settings
#define FAST_BLINK_RATE 150 // toggle every 250 ms
#define REGULAR_BLINK_RATE 750 // toggle every 750 ms

// Pressure Charge Settings
// #define PRESSURE_ALWAYS_ON
#define PRESSURE_READ_RATE 10 // ms between reads of pressure sensor
#define MAX_CHARGE_PRESSURE 83000 // milli-inH20 ( 83000 = 3 PSI )
#define MAX_CHARGE_TIMEOUT 60000 // ms
#define PRESSURE_SENSOR_DELTA_P 138400 // Pmax-Pmin, 55.360 inH2O (2psi) 138.400 inH2O (5psi) 276.800 inH2O (10psi)
#define PRESSURE_SENSOR_PMIN 0 // Pmin 27.680 inH2O (1psi) 138.400 inH2O (5psi)
#define PRESSURE_SENSOR_VSUPPLY 3000 // 3.3 volts
#define DELAY_FOR_PUMP_TO_CHARGING_TRANSITION 500
#define DELAY_FOR_CHARGING_TO_PUMP_TRANSITION 500

// Pail Sensor Settings
#define DEFAULT_PAIL_DETECTION_THRESHOLD 600 // ADC Value (out of 4085)
#define PAIL_SENSOR_ENABLED // Remove to Disable the Pail Sensor

// Mix Master Settings
#define TIME_BETWEEN_KEEP_OPEN_CYCLES 300000 // 5 minutes
#define FLUSH_FLOW_RATE 3000 // ml/min
#define FLUSH_VOLUME_PER_PULSE 200 //ml
#define FLUSH_PULSE_INTERVAL 60000 // time bewteen start of pulses (1 min)
#define FLUSH_CYCLE_DURATION 600000 // duration of flush cycle (10 min)

// Button and Remote Settings
// time must be held down to be a long press to initiate a "Flush" cycle
#define LONG_PRESS_TIME 5000

//  EEPROM Structure Definition
//    When changing the structure of EEPROM, increase THIS_EEPROM_VERSION, this
//    will cause EEPROM of previous versions to reset to default.
#define THIS_EEPROM_VERSION 8

struct prom_settings {
  uint32_t version;
  uint32_t stepsPerMlResin;
  uint32_t stepsPerMlHardener;
  uint32_t autoReverseSteps;
  uint32_t onPressure;
  uint32_t offPressure;
  uint32_t chargeTimeout;
  uint32_t chargeOffTime;
  uint32_t chargeDelay;
  uint32_t keepOpenVolume;
  uint32_t pailThreshold;
  uint32_t maxPreMixChargingTime;
  uint32_t maxNoPressure;
  uint32_t primeVolume;
  uint32_t minPrimes;
  // Selector Based Settings
  uint32_t flowRate[NUM_SELECTORS];
  uint32_t ratioResin[NUM_SELECTORS];
  uint32_t ratioHardener[NUM_SELECTORS];
  uint32_t volume[NUM_SELECTORS];
};

const uint32_t settingsAddr = 0;

const prom_settings defaultSettings = {
  THIS_EEPROM_VERSION,
  33, // stepsPerMlResin
  33, // stepsPerMlHardener
  175, // autoReverseSteps
  30448, // onPressure
  44288, // offPressure
  15000, // chargeTimeout
  15000, // chargeOffTime
  1000, // chargeDelay
  150, // keepOpenVolume
  2000, // pailThreshold
  1000, // maxPreMixChargingTime
  1000, // maxNoPressure
  750, // primeVolume
  1, // minPrimes
  {3000,3000,3000,3000,3000}, // flowRate
  {0,200,100,224,189}, // ratioResin
  {0,100,400,100,100}, // ratioHardener
  {3785,3785,3785,3785,3785} // volume
};

#endif
