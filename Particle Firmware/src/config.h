#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

#define THIS_PRODUCT_ID 7951
#define THIS_PRODUCT_VERSION 20

// Photon Pins
#define SDA_PIN D0
#define SCL_PIN D1
#define RESIN_INPUT_DIR_PIN D2
#define RESIN_INPUT_STEP_PIN D3
#define HARDENER_INPUT_DIR_PIN D4
#define HARDENER_INPUT_STEP_PIN D5

#define PAIL_SNS_PIN A0
#define PRESS_SNS_PIN A1
#define THERM_RESIN_PIN A2
#define THERM_HARDENER_PIN A3
#define STATUS_LED_R A4
#define STATUS_LED_G A5
#define STATUS_LED_B WKP
#define CHIME_PIN RX

// IOExp Pins
#define USER_BUTTON_IOEXP_PIN 0
#define USER_REMOTE_IOEXP_PIN 1
#define LIQUID_SNS_HARDENER_IOEXP_PIN 2
#define LIQUID_SNS_RESIN_IOEXP_PIN 3
#define PUMP_EN_IOEXP_PIN 4
#define LED_HARD_LIQ_IOEXP_PIN 5
#define LED_RESIN_LIQ_IOEXP_PIN 6

#define RESIN_ENABLE_IOEXP_PIN 8
#define RESIN_HLFB_IOEXP_PIN 9
#define HARDENER_ENABLE_IOEXP_PIN 10
#define HARDENER_HLFB_IOEXP_PIN 11
#define ROTARY_4_IOEXP_PIN 12
#define ROTARY_3_IOEXP_PIN 13
#define ROTARY_2_IOEXP_PIN 14
#define ROTARY_1_IOEXP_PIN 15

// Selector Switch Config
#define NUM_SELECTORS 5

// Status LED Settings
#define FAST_BLINK_RATE 150 // toggle every 250 ms
#define REGULAR_BLINK_RATE 750 // toggle every 750 ms

// Chime Settings
#define CHIME_FREQUENCY 3100 // 1100, 3100
#define CHIME_PULSE_TIME 250
#define CHIME_SLOW_RATE 3000
#define CHIME_MEDIUM_RATE 1500
#define CHIME_FAST_RATE 1000

// Temperature Monitor Settings
#define TEMPERATURE_CHECK_RATE 5000 // ms - temperature checking is slow, avoid calling frequently
#define TEMPERATURE_BETA_VALUE 3950
#define TEMPERATURE_FIXED_RESISTOR 4990 // ohms
#define TEMPERATURE_NOM_RESISTANCE 5000 // ohms
#define TEMPERATURE_NOM_CELCIUS 25.0 // degrees c
#define TEMPERATURE_NUM_SAMPLES 100 // number of adc samples in rolling average
#define TEMPERATURE_SAMPLE_RATE 10 // time between adc samples

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

// Mix Master Settings
#define TIME_BETWEEN_KEEP_OPEN_CYCLES 300000 // 5 minutes
#define FLUSH_FLOW_RATE 2000 // ml/min
#define FLUSH_MIN_TIME 30000 // ms
#define FLUSH_MAX_TIME 60000 // ms

// Pump Configuration
#define STEPS_PER_REV 200 

// Button and Remote Settings
// time must be held down to be a long press to initiate a "Flush" cycle
#define LONG_PRESS_TIME 3000

//  EEPROM Structure Definition
//    When changing the structure of EEPROM, increase THIS_EEPROM_VERSION, this
//    will cause EEPROM of previous versions to reset to default.
#define THIS_EEPROM_VERSION 16

struct prom_settings {
  uint32_t version;
  uint32_t stepsPerMlResin;
  uint32_t stepsPerMlHardener;
  uint32_t autoReverseSteps;
  uint32_t onPressure;
  uint32_t offPressure;
  uint32_t chargeTimeout;
  uint32_t chargeDelay;
  uint32_t keepOpenVolume;
  uint32_t pailThreshold;
  uint32_t maxPreMixChargingTime;
  uint32_t maxNoPressure;
  uint32_t primeVolume;
  uint32_t minPrimes;
  uint32_t minTemperature;
  uint32_t maxTemperature;
  uint32_t flushInitialBolusVolume;
  uint32_t flushVolume;
  uint32_t flushRPM;
  uint32_t flushForwardSteps;
  uint32_t flushReverseSteps;
  uint32_t flushForwardPause;
  uint32_t flushReversePause;
  uint32_t zeroDripSteps;
  uint32_t zeroDripInterval;
  uint32_t maxTimeNoLiquid;
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
  1000, // chargeDelay
  150, // keepOpenVolume
  2000, // pailThreshold
  1000, // maxPreMixChargingTime
  1000, // maxNoPressure
  750, // primeVolume
  1, // minPrimes
  50, // minTemperature
  90, // maxTemperature
  400, // flushInitialBolusVolume
  2000, // flushVolume
  200, // flushRPM
  200, // flushForwardSteps
  180, // flushReverseSteps
  1000, // flushForwardPause
  1000, // flushReversePause
  5, // zeroDripSteps
  1000, // zeroDripInterval
  3000, // maxTimeNoLiquid
  {3785,3785,3785,3785,3785}, // flowRate
  {0,200,100,231,204}, // ratioResin
  {0,100,400,100,100}, // ratioHardener
  {3785,3785,3785,3785,3785} // volume
};

#endif
