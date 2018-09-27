#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

#define THIS_PRODUCT_ID 7951
#define THIS_PRODUCT_VERSION 7

#define RESIN_PUMP_ENABLE_PIN D0
#define RESIN_PUMP_DIR_PIN D1
#define RESIN_PUMP_STEP_PIN D2
#define RESIN_PUMP_ASSERT_PIN D3

#define HARDENER_PUMP_ENABLE_PIN D4
#define HARDENER_PUMP_DIR_PIN D5
#define HARDENER_PUMP_STEP_PIN D7
#define HARDENER_PUMP_ASSERT_PIN D6

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
#define FAST_BLINK_RATE 250 // toggle every 250 ms
#define REGULAR_BLINK_RATE 750 // toggle every 750 ms

// Pressure Charge Settings
#define MAX_CHARGE_PRESSURE 25000
#define PRESSURE_SENSOR_DELTA_P 55360 // Pmax-Pmin, 55.360 inH2O (2psi)
#define PRESSURE_SENSOR_PMIN -27680 // Pmin 27.680 inH2O (1psi)
#define PRESSURE_SENSOR_VSUPPLY 3300 // 3.3 volts

// Pail Sensor Settings
#define DEFAULT_PAIL_DETECTION_THRESHOLD 600 // ADC Value (out of 4085)
#define PAIL_SENSOR_ENABLED // Remove to Disable the Pail Sensor

// Mix Master Settings
#define TIME_BETWEEN_KEEP_OPEN_CYCLES 300000 // 5 minutes
#define FLUSH_FLOW_RATE 3000 // ml/min
#define FLUSH_VOLUME_PER_PULSE 200 //ml
#define FLUSH_PULSE_INTERVAL 10000 // time bewteen start of pulses (10 sec)
#define FLUSH_CYCLE_DURATION 600000 // duration of flush cycle (10 min)

// Button and Remote Settings
// time must be held down to be a long press to initiate a "Flush" cycle
#define LONG_PRESS_TIME 5000

//  EEPROM Structure Definition
//    When changing the structure of EEPROM, increase THIS_EEPROM_VERSION, this
//    will cause EEPROM of previous versions to reset to default.
#define THIS_EEPROM_VERSION 4

struct prom_settings {
  uint32_t version;
  uint32_t stepsPerMlResin;
  uint32_t stepsPerMlHardener;
  uint32_t autoReverseSteps;
  uint32_t chargePressure;
  uint32_t keepOpenVolume;
  uint32_t pailThreshold;
  // Selector Based Settings
  uint32_t flowRate[NUM_SELECTORS];
  uint32_t ratioResin[NUM_SELECTORS];
  uint32_t ratioHardener[NUM_SELECTORS];
  uint32_t volume[NUM_SELECTORS];
};

const uint32_t settingsAddr = 0;

const prom_settings defaultSettings = {
  THIS_EEPROM_VERSION,
  128, // stepsPerMlResin
  128, // stepsPerMlHardener
  500, // autoReverseSteps
  10000, // chargePressure
  150, // keepOpenVolume
  600, // pailThreshold
  {3000,3000,3000,3000,3000}, // flowRate
  {200,200,200,200,200}, // ratioResin
  {100,100,100,100,100}, // ratioHardener
  {3785,3785,3785,3785,3785} // volume
};

#endif
