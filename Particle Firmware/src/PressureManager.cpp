#include "PressureManager.h"

pressureManager PressureManager;


// Initialize air pump and pressure sensor
void pressureManager::init(uint32_t onPressure, uint32_t offPressure){
  updateTargetPressure(onPressure,1);
  updateTargetPressure(offPressure,0);
  IOExp.pinMode(PUMP_EN_IOEXP_PIN, OUTPUT);
  pinMode(PRESS_SNS_PIN, INPUT);

  IOExp.digitalWrite(PUMP_EN_IOEXP_PIN, LOW);
}

void pressureManager::update(){
  // Take readings, reset index and make valid if _pressure.samples is full
  if(millis()-_pressure.lastRead > PRESSURE_READ_RATE){
    _pressure.samples[_pressure.index++] = this->readPressure();
    _pressure.lastRead = millis();
  }
  if(_pressure.index > _pressure.length){
    _pressure.index = 0;
    _pressure.isValid = true;
  }

  // Calculate average and convert to milli-inH2O
  if(_pressure.isValid){
    uint32_t sumOfSamples = 0;
    for(unsigned int i=0; i<_pressure.length; i++){
      sumOfSamples += _pressure.samples[i];
    }
    uint32_t averageRaw = sumOfSamples/_pressure.length;
    averageRaw = averageRaw*3300/4095; // convert to milli-volts

    // Convert to milli-inH2O
    _pressure.current = PRESSURE_SENSOR_PMIN + ((PRESSURE_SENSOR_DELTA_P*(averageRaw - (0.1*PRESSURE_SENSOR_VSUPPLY)))/(0.8*PRESSURE_SENSOR_VSUPPLY));
  }

  if(_pressure.isValid){
    if(_pressure.current < _onPressure ||
      (_pressure.current < _offPressure && _atPressure == false)){
        // Set state to under pressure state and request charging
        _atPressure = false;
        _requestCharging = true;

        // Charge if allowed
        if(_allowCharging && !_currentChargingState){
          IOExp.digitalWrite(PUMP_EN_IOEXP_PIN, HIGH);
          _currentChargingState = true;
        } else if(!_allowCharging && _currentChargingState){
          IOExp.digitalWrite(PUMP_EN_IOEXP_PIN, LOW);
          _currentChargingState = false;
        }
    } else if(_pressure.current > _offPressure){
      // set atPressure and stop requesting charging
      _atPressure = true;
      _requestCharging = false;

      // disable air pump
      if(_currentChargingState == true){
        IOExp.digitalWrite(PUMP_EN_IOEXP_PIN, LOW);
        _currentChargingState = false;
      }
    }
  }

  // Check to make sure Allow Charging was called in last 10ms, reset if not
  if(millis() - _lastAllowChargingCall > 10){
    _allowCharging = false;
  }
}

void pressureManager::allowCharging(){
  _allowCharging = true;
  _lastAllowChargingCall = millis();
}

bool pressureManager::requestCharging(){
  return _requestCharging;
}

bool pressureManager::isCharged(){
  return !_requestCharging;
}

void pressureManager::forceAtPressure(){
  _atPressure = true;
}

void pressureManager::forceUnderPressure(){
  _atPressure = false;
}

int32_t pressureManager::updateTargetPressure(int32_t pressure, bool type){
  if(pressure > MAX_CHARGE_PRESSURE){
    pressure = MAX_CHARGE_PRESSURE;
  }

  if(type == 1) this->_onPressure = pressure; // on Pressure
  else _offPressure = pressure; // off Pressure

  return pressure;
}

int32_t pressureManager::getPressure(){
  if(_pressure.isValid) return _pressure.current;
  return 0;
}

uint16_t pressureManager::readPressure(){
  return analogRead(PRESS_SNS_PIN);
}
