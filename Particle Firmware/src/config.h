#ifndef CONFIG_H
#define CONFIG_H

#define THIS_PRODUCT_ID 7951
#define THIS_PRODUCT_VERSION 5

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

//  EEPROM Structure Definition
//    When changing the structure of EEPROM, increase THIS_EEPROM_VERSION, this
//    will cause EEPROM of previous versions to reset to default.
#define THIS_EEPROM_VERSION 2

struct prom_settings {
  uint32_t version;
  uint32_t stepsPerMlA;
  uint32_t stepsPerMlB;
  // Selector Based Settings
  uint32_t flowRate[NUM_SELECTORS];
  uint32_t ratioA[NUM_SELECTORS];
  uint32_t ratioB[NUM_SELECTORS];
  uint32_t autoReverseA[NUM_SELECTORS];
  uint32_t autoReverseB[NUM_SELECTORS];
  uint32_t volume[NUM_SELECTORS];
};

const uint32_t settingsAddr = 0;

const prom_settings defaultSettings = {
  THIS_EEPROM_VERSION,
  128, // stepsPerMlA
  128, // stepsPerMlB
  {3000,3000,3000,3000,3000}, // flowRate
  {200,200,200,200,200}, // ratioA
  {100,100,100,100,100}, // ratioB
  {150,150,150,150,150}, // autoReverseA
  {75,75,75,75,75}, // autoReverseB
  {3785,3785,3785,3785,3785} // volume
};

#endif
