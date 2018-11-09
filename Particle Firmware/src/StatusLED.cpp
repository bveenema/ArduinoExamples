#include "StatusLED.h"

statusLED StatusLED;

statusLED::statusLED(){}

void statusLED::init(){
  // Take control of the system RGB LED and turn off
  RGB.control(true);
  RGB.color(0,0,0);

  // Setup Red LEDs
  IOExp.pinMode(RedLED_1.pin, OUTPUT);
  IOExp.pinMode(RedLED_2.pin, OUTPUT);
  RedLED_1.on();
  RedLED_2.on();
}

void statusLED::update(){
  MixerState mixState = MixMaster.getState();

  // Check externals to decide LED state
       if(!PailSensor.getState())       this->set(GREEN, FAST_BLINK);
  else if(!isPrimed)                    this->set(GREEN, BLINK);
  else if(mixState == FLUSHING)         this->set(GREEN, BLINK);
  else                                  this->set(GREEN, ON);

  // call functions based on current state
       if(currentState == FAST_BLINK)   this->blinkRGB(FAST_BLINK_RATE);
  else if(currentState == BLINK)        this->blinkRGB(REGULAR_BLINK_RATE);
  else if(currentState == ON)           RGB.brightness(255);
  else                                  RGB.brightness(0);

  // set color
       if(currentColor == RED)          RGB.color(255,0,0);
  else if(currentColor == GREEN)        RGB.color(0,255,0);
  else if(currentColor == BLUE)         RGB.color(0,0,255);
  else if(currentColor == YELLOW)       RGB.color(255,255,0);
  else if(currentColor == CYAN)         RGB.color(0,255,255);
  else if(currentColor == MAGENTA)      RGB.color(255,0,255);
  else                                  RGB.color(0,0,0);

  // Check Liquid Sensors and mixing state to decide Red LED states
       if(!ResinLiquidSensor.hasLiquid())     RedLED_1.blink(FAST_BLINK_RATE);
  else if(mixState == MIXING)                 RedLED_1.on();
  else                                        RedLED_1.off();

       if(!HardenerLiquidSensor.hasLiquid())  RedLED_2.blink(FAST_BLINK_RATE);
  else if(mixState == MIXING)                 RedLED_2.on();
  else                                        RedLED_2.off();

}

void statusLED::set(RGBColor color, LEDState state){
  currentState = state;
  currentColor = color;
}

void statusLED::blinkRGB(uint32_t rate){
  if(millis() - lastBlinkTime > rate) {
    if(RGB.brightness() > 125) RGB.brightness(0);
    else RGB.brightness(255);
    lastBlinkTime = millis();
  }
}
