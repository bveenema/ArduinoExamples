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
  if(millis() - _lastRead > 250){
    _lastRead = millis();
    bool hasLiquid = !IOExp.digitalRead(_pin);
    if(!hasLiquid){
      if(millis() - _firstTimeNoLiquid > _maxTimeNoLiquid) _hasLiquid = false;
    } else {
      _hasLiquid = true;
      _firstTimeNoLiquid = millis();
    }
  }
}

void liquidSensor::setMaxTimeNoLiquid(uint32_t time){
  time = constrain(time, 250, 240000);
  _maxTimeNoLiquid = time;
}

bool liquidSensor::hasLiquid(){
  return _hasLiquid;
}
