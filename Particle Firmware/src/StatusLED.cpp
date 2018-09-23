#include "StatusLED.h"

statusLED::statusLED(){}

void statusLED::init(){
  pinMode(STATUS_LED_PIN,OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);
}

void statusLED::update(){
  if(strcmp(currentError, "none") != 0) currentState = FAST_BLINK;
  else if(MixMaster.getState() == Cleaning) currentState = BLINK;
  else if(MixMaster.getState() == Mixing) currentState = ON;
  else currentState = OFF;

  if(currentState == FAST_BLINK) this->blink(FAST_BLINK_RATE);
  else if(currentState == BLINK) this->blink(REGULAR_BLINK_RATE);
  else if(currentState == ON) pinSetFast(STATUS_LED_PIN);
  else pinResetFast(STATUS_LED_PIN);
}


void statusLED::blink(uint32_t rate){
  if(millis() - lastBlinkTime > rate) {
    digitalWriteFast(STATUS_LED_PIN, !pinReadFast(STATUS_LED_PIN));
    lastBlinkTime = millis();
  }
}
