#ifndef STATUS_LED_H
#define STATUS_LED_H

#include "Particle.h"
#include "config.h"
#include "globals.h"
#include "MixMaster.h"

extern mixMaster MixMaster;

typedef enum{
  OFF,
  ON,
  BLINK,
  FAST_BLINK,
} LEDState;

class statusLED{
public:
  statusLED();

  // Initiates Pins
  void init();

  // Checks MixMaster state and currentError to decide LEDState
  //  * Hierarchy:  0 - Motor Error
  //                1 - Flushing
  //                2 - Mixing
  //                3 - Off
  void update();

private:
  LEDState currentState = OFF;
  uint32_t lastBlinkTime = 0;

  // toggles LED if time is expired
  void blink(uint32_t rate);
};

#endif