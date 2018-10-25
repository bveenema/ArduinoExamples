#include "PressureManager.h"

pressureManager PressureManager;


// Initialize air pump and pressure sensor
void pressureManager::init(uint32_t onPressure, uint32_t offPressure){
  updateTargetPressure(onPressure,1);
  updateTargetPressure(offPressure,0);
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
      for(unsigned int i=0; i<pressure.length; i++){
        sumOfSamples += pressure.samples[i];
      }
      uint32_t averageRaw = sumOfSamples/pressure.length;
      averageRaw = averageRaw*3300/4095; // convert to milli-volts

      // Convert to milli-inH2O
      pressure.current = PRESSURE_SENSOR_PMIN + ((PRESSURE_SENSOR_DELTA_P*(averageRaw - (0.1*PRESSURE_SENSOR_VSUPPLY)))/(0.8*PRESSURE_SENSOR_VSUPPLY));
    }

    // Evaluate
    if(this->isCharging && pressure.isValid) {
      if(pressure.current < this->onPressure ||
         ((pressure.current < this->offPressure) && (this->atPressure == false))){
          // Set state to under pressure state
          this->atPressure = false;

          // request charging
          requestCharging = true;

          // Charge if allowed or reset chargingTimer
          if(allowCharging) pinSetFast(AIR_PUMP_EN);
          else {
            pinResetFast(AIR_PUMP_EN);
            chargingTimer = millis();
          }

          // if charging timeout, set requestCharging to false
          if(millis() - chargingTimer > chargingTimeout){
            requestCharging = false;
          }

          // Accumulate error while under target pressure
          if(requestCharging) pressure.accumulateUnderPressure += 1;
          if(pressure.accumulateUnderPressure > 10000) {
            this->charged = false;
            pressure.accumulateUnderPressure = 10001;
          }
      } else if(pressure.current > this->offPressure){

        // set state to over pressure and turn of pump
        this->atPressure = true;
        pinResetFast(AIR_PUMP_EN);

        // set state to charged, reset under pressure error
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

int32_t pressureManager::updateTargetPressure(int32_t pressure, bool type){
  if(pressure > MAX_CHARGE_PRESSURE){
    pressure = MAX_CHARGE_PRESSURE;
  }

  if(type == 1) this->onPressure = pressure; // on Pressure
  else this->offPressure = pressure; // off Pressure

  return pressure;
}


uint32_t pressureManager::updateChargeTimeout(uint32_t timeout){
  if(timeout > MAX_CHARGE_TIMEOUT){
    timeout = MAX_CHARGE_TIMEOUT;
  }
  this->chargingTimeout = timeout;
  return this->chargingTimeout;
}

int32_t pressureManager::getPressure(){
  if(pressure.isValid) return pressure.current;
  return 0;
}

uint16_t pressureManager::readPressure(){
  return analogRead(PRESS_SNS_PIN);
}
