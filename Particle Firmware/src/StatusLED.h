#ifndef STATUS_LED_H
#define STATUS_LED_H

#include "Particle.h"
#include "config.h"
#include "globals.h"
#include "MixMaster.h"
#include "PailSensor.h"
#include "LiquidSensor.h"

extern mixMaster MixMaster;

typedef enum{
  OFF,
  ON,
  BLINK,
  FAST_BLINK,
} LEDState;

typedef enum{
  RED,
  GREEN,
  BLUE,
  YELLOW,
  CYAN,
  MAGENTA,
} RGBColor;

class statusLED{
public:
  statusLED();

  // Initiates Pins
  void init();

  // Checks MixMaster state and currentError to decide LEDState
  //  * Hierarchy:  0 - No Bucket - Fast Blink
  //                1 - Not Primed - Slow Blink
  //                2 - Flushing - Slow Blink
  //                3 - Ready to Mix - Solid On
  void update();

private:
  LEDState currentState = OFF;
  RGBColor currentColor = GREEN;
  uint32_t lastBlinkTime = 0;

  // State holders for the Red Status LED's
  struct RedLED{
    bool state;
    pin_t pin;
    uint32_t lastBlinkTime;
    void on(){
      state = true;
      IOExp.digitalWrite(pin, HIGH);
    }
    void blink(uint32_t rate){
      if(millis()-lastBlinkTime > rate){
        state = !state;
        IOExp.digitalWrite(pin, state);
        lastBlinkTime = millis();
      }
    }
  };
  RedLED RedLED_1 = {0, LED_RESIN_LIQ_IOEXP_PIN, 0};
  RedLED RedLED_2 = {0, LED_HARD_LIQ_IOEXP_PIN, 0};

  void set(RGBColor color, LEDState state);

  // toggles LED if time is expired
  void blinkRGB(uint32_t rate);
};

extern statusLED StatusLED;

#endif
