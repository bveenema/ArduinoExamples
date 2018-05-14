#pragma once

#define THIS_PRODUCT_ID 1111
#define THIS_PRODUCT_VERSION 1
// PRODUCT_ID(5864);
// PRODUCT_VERSION(1);

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

struct prom {
  bool version;
  uint16_t flowRate;
  uint16_t ratioA;
  uint16_t ratioB;
  uint32_t autoReverse;
  uint16_t stepsPerMlA;
  uint16_t stepsPerMlB;
};

const uint32_t settingsAddr = 0;

const prom default_settings = {
  0,
  3000,
  200,
  100,
  1000,
  138,
  138
};
