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

    // Take readings, reset index and make valid if pressure.samples is full
    pressure.samples[pressure.index++] = this->readPressure();
    if(pressure.index > pressure.length){
      pressure.index = 0;
      pressure.isValid = true;
    }

    // Calculate average and convert to milli-inH2O
    if(pressure.isValid){
      uint32_t sumOfSamples = 0;
      for(int i=0; i<pressure.length; i++){
        sumOfSamples += pressure.samples[i];
      }
      uint32_t averageRaw = sumOfSamples/pressure.length;
      averageRaw = averageRaw*3300/4095; // convert to milli-volts

      // Convert to milli-inH2O
      pressure.current = PRESSURE_SENSOR_PMIN + ((PRESSURE_SENSOR_DELTA_P*(averageRaw - (0.1*PRESSURE_SENSOR_VSUPPLY)))/(0.8*PRESSURE_SENSOR_VSUPPLY));
    }

    // Evaluate
    if(this->isCharging) {
      if(pressure.isValid && (pressure.current < this->targetPressure)){
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
  if(pressure.isValid) return pressure.current;
  return 0;
}

uint16_t pressureManager::readPressure(){
  return analogRead(PRESS_SNS_PIN);
}
