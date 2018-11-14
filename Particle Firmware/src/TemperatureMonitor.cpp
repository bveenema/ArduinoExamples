#include "TemperatureMonitor.h"

TemperatureMonitor ResinTemp;
TemperatureMonitor HardenerTemp;

TemperatureMonitor::TemperatureMonitor(){}

void TemperatureMonitor::init(pin_t pin){
  _pin = pin;
  thermistor.init(pin, TEMPERATURE_FIXED_RESISTOR, TEMPERATURE_NOM_RESISTANCE, TEMPERATURE_NOM_CELCIUS, TEMPERATURE_BETA_VALUE, TEMPERATURE_NUM_SAMPLES, TEMPERATURE_SAMPLE_RATE);
}

void TemperatureMonitor::update(){
  thermistor.update();

  if(thermistor.isValid()){
    if(millis() - _lastTemperatureCheck > TEMPERATURE_CHECK_RATE){
      _lastTemperatureCheck = millis();
      _currentTemp = (int32_t)thermistor.readTempF(); // readTemp is slow, avoid calling frequently
    }
  }
}

uint32_t TemperatureMonitor::getTemp(){
  return _currentTemp;
}

bool TemperatureMonitor::isInRange(){
  // if readings are not yet valid, assume in range
  if(!thermistor.isValid()) return true;
  return (_currentTemp >= _minTemp && _currentTemp <= _maxTemp);
}

void TemperatureMonitor::setRange(int32_t min, int32_t max){
  if(max < min) return;
  _minTemp = min;
  _maxTemp = max;
}
