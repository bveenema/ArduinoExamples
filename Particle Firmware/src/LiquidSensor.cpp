#include "LiquidSensor.h"

liquidSensor ResinLiquidSensor;
liquidSensor HardenerLiquidSensor;

liquidSensor::liquidSensor(){}

void liquidSensor::init(uint8_t pin){
  _pin = pin;
  // Setup pin
  IOExp.pinMode(_pin, INPUT);
}

void liquidSensor::update(){
  // If signal is low, there is no liquid
  if(millis() - _lastRead > 1000){
    _lastRead = millis();
    _hasLiquid = !IOExp.digitalRead(_pin);
  }
}

bool liquidSensor::hasLiquid(){
  return _hasLiquid;
}
