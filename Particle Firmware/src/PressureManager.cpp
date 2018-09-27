#include "PressureManager.h"

pressureManager::pressureManager(uint32_t _targetPressure){
  updateTargetPressure(_targetPressure);
}

// Initialize air pump and pressure sensor
void pressureManager::init(){
  pinMode(AIR_PUMP_EN, OUTPUT);
  pinMode(PRESS_SNS_PIN, INPUT);

  digitalWrite(AIR_PUMP_EN, LOW);
}

void pressureManager::update(){
  if(this->isCharging) {
    int32_t currentPressure = this->readPressure();
    if(currentPressure < this->targetPressure){
      pinSetFast(AIR_PUMP_EN);
    }else{
      pinResetFast(AIR_PUMP_EN);
      this->charged = true;
    }
  } else {
    pinResetFast(AIR_PUMP_EN);
    this->charged = false;
  }
}

void pressureManager::setChargingState(bool charging){
  this->isCharging = charging;
}

bool pressureManager::isCharged(){
  return this->charged;
}

int32_t pressureManager::updateTargetPressure(int32_t _targetPressure){
  if(_targetPressure > MAX_CHARGE_PRESSURE){
    _targetPressure = MAX_CHARGE_PRESSURE;
  }
  this->targetPressure = _targetPressure;
  return this->targetPressure;
}

int32_t pressureManager::getPressure(){
  return readPressure();
}

int32_t pressureManager::readPressure(){
  int32_t pressureRaw = analogRead(PRESS_SNS_PIN)*3300/4095; // conver 12bit ADC to millivolts
  //Apply transfer function (https://sensing.honeywell.com/index.php?ci_id=151133 page 11)
  return PRESSURE_SENSOR_PMIN + ((PRESSURE_SENSOR_DELTA_P*(pressureRaw - (0.1*PRESSURE_SENSOR_VSUPPLY)))/(0.8*PRESSURE_SENSOR_VSUPPLY));
}
