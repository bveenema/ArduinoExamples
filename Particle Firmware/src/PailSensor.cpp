#include "PailSensor.h"
pailSensor PailSensor;

pailSensor::pailSensor(){}

void pailSensor::init(){
  pinMode(PAIL_SNS_PIN, INPUT);
}

void pailSensor::update(){
  // Take readings, reset index and make valid if reading.values is full
  readings.values[readings.index++] = this->readSensor();
  if(readings.index > readings.length) {
    readings.index = 0;
    readings.areValid = true;
  }

  // Calculate Average
  if(readings.areValid) {
    uint32_t sumOfReadings = 0;
    for(int i=0; i<readings.length; i++){
      sumOfReadings += readings.values[i];
    }
    currentReading = sumOfReadings/readings.length;
  }

  // Evaluate
  if(readings.areValid && (currentReading > detectionThreshold)) this->state = true;
  else this->state = false;
}

void pailSensor::setDetectionThreshold(uint32_t newThreshold){
  if(newThreshold > 4085) return;
  detectionThreshold = newThreshold;
}

uint32_t pailSensor::getCurrentReading(){
  if(readings.areValid) return this->currentReading;
  return 0;
}

bool pailSensor::getState(){
  return this->state;
}

uint16_t pailSensor::readSensor(){
  return analogRead(PAIL_SNS_PIN);
}
