#include "PressureManager.h"

pressureManager PressureManager;


// Initialize air pump and pressure sensor
void pressureManager::init(uint32_t targetPressure){
  updateTargetPressure(targetPressure);
  pinMode(AIR_PUMP_EN, OUTPUT);
  pinMode(PRESS_SNS_PIN, INPUT);

  digitalWrite(AIR_PUMP_EN, LOW);
}

bool pressureManager::update(bool allowCharging){
  bool requestCharging = false;
    // Take readings, reset index and make valid if pressure.samples is full
    if(millis()-pressure.lastRead > PRESSURE_READ_RATE){
      pressure.samples[pressure.index++] = this->readPressure();
      pressure.lastRead = millis();
    }
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
        requestCharging = true;
        if(allowCharging){
          pinSetFast(AIR_PUMP_EN);
        }
        pressure.accumulateUnderPressure += 1;
        if(pressure.accumulateUnderPressure > 1000) {
          this->charged = false;
          pressure.accumulateUnderPressure = 1001;
        }
      }else{
        pinResetFast(AIR_PUMP_EN);
        this->charged = true;
        pressure.accumulateUnderPressure = 0;
      }
    } else {
      pinResetFast(AIR_PUMP_EN);
      this->charged = false;
    }

    return requestCharging;
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
