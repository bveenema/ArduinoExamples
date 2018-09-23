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
#define HARDENER_PUMP_STEP_PIN D6
#define HARDENER_PUMP_ASSERT_PIN D7

#define PRESS_SNS_PIN A0
#define PAIL_SNS_PIN A1
#define PUMP_EN A2

#define BUTTON_PIN A3
#define STATUS_LED_PIN A4
#define REMOTE_PIN A5

#define SELECTOR_SWITCH_4 DAC
#define SELECTOR_SWITCH_3 WKP
#define SELECTOR_SWITCH_2 RX
#define SELECTOR_SWITCH_1 TX

#define NUM_SELECTORS 5

// Mix Master Settings
#define TIME_BETWEEN_FLUSHES 300000 // 5 minutes
#define FLUSH_VOLUME 150 //ml
#define CLEANING_FLOW_RATE 3000 // ml/min
#define CLEANING_VOLUME_PER_PULSE 200 //ml
#define CLEANING_PULSE_INTERVAL 10000 // time bewteen start of pulses (10 sec)
#define CLEANING_CYCLE_DURATION 600000 // duration of cleaning cycle (10 min)

// Button and Remote Settings
// time must be held down to be a long press to initiate a "Cleaning" cycle
#define LONG_PRESS_TIME 5000

//  EEPROM Structure Definition
//    When changing the structure of EEPROM, increase THIS_EEPROM_VERSION, this
//    will cause EEPROM of previous versions to reset to default.
#define THIS_EEPROM_VERSION 2

struct prom_settings {
  uint32_t version;
  uint32_t stepsPerMlResin;
  uint32_t stepsPerMlHardener;
  // Selector Based Settings
  uint32_t flowRate[NUM_SELECTORS];
  uint32_t ratioResin[NUM_SELECTORS];
  uint32_t ratioHardener[NUM_SELECTORS];
  uint32_t autoReverseResin[NUM_SELECTORS];
  uint32_t autoReverseHardener[NUM_SELECTORS];
  uint32_t volume[NUM_SELECTORS];
};

const uint32_t settingsAddr = 0;

const prom_settings defaultSettings = {
  THIS_EEPROM_VERSION,
  128, // stepsPerMlResin
  128, // stepsPerMlHardener
  {3000,3000,3000,3000,3000}, // flowRate
  {200,200,200,200,200}, // ratioResin
  {100,100,100,100,100}, // ratioHardener
  {150,150,150,150,150}, // autoReverseResin
  {75,75,75,75,75}, // autoReverseHardener
  {3785,3785,3785,3785,3785} // volume
};

#endif
