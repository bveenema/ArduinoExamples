#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

#define THIS_PRODUCT_ID 7951
#define THIS_PRODUCT_VERSION 7

#define MOTOR_RESIN_ENABLE_PIN D0
#define MOTOR_RESIN_DIR_PIN D1
#define MOTOR_RESIN_STEP_PIN D2
#define MOTOR_RESIN_ASSERT_PIN D3

#define MOTOR_HARDENER_ENABLE_PIN D4
#define MOTOR_HARDENER_DIR_PIN D5
#define MOTOR_HARDENER_STEP_PIN D6
#define MOTOR_HARDENER_ASSERT_PIN D7

#define BUTTON_PIN A0
#define ERROR_LED_PIN A1
#define REMOTE_PIN A2

#define NUM_SELECTORS 5

#define SELECTOR_SWITCH_1 A3
#define SELECTOR_SWITCH_2 A4
#define SELECTOR_SWITCH_3 A5
#define SELECTOR_SWITCH_4 A6

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
