#pragma once

#define THIS_PRODUCT_ID 7951
#define THIS_PRODUCT_VERSION 5
PRODUCT_ID(THIS_PRODUCT_ID);
PRODUCT_VERSION(THIS_PRODUCT_VERSION);

#define MOTORA_ENABLE_PIN D0
#define MOTORA_DIR_PIN D1
#define MOTORA_STEP_PIN D2
#define MOTORA_ASSERT_PIN D3

#define MOTORB_ENABLE_PIN D4
#define MOTORB_DIR_PIN D5
#define MOTORB_STEP_PIN D6
#define MOTORB_ASSERT_PIN D7

#define BUTTON_PIN A0
#define ERROR_LED_PIN A1
#define REMOTE_PIN A2

#define NUM_SELECTORS 5

#define SELECTOR_SWITCH_1 A3
#define SELECTOR_SWITCH_2 A4
#define SELECTOR_SWITCH_3 A5
#define SELECTOR_SWITCH_4 A6

struct prom_SelectorSettings {
  uint16_t flowRate;
  uint16_t ratioA;
  uint16_t ratioB;
  uint32_t autoReverseA;
  uint32_t autoReverseB;
  uint32_t volume;
};

struct prom_MotorSettings {
  uint16_t stepsPerMlA;
  uint16_t stepsPerMlB;
};

struct prom_AllSettings {
  bool version;
  prom_SelectorSettings selectorSettings[5];
  prom_MotorSettings motorSettings;
};

const uint32_t settingsAddr = 0;

const prom_SelectorSettings defaultSelectorSettings = {
  3000,
  200,
  100,
  0,
  0,
  3785
};

const prom_MotorSettings defaultMotorSettings = {
  128,
  128
};
