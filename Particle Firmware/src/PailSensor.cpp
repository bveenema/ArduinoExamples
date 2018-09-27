#include "PailSensor.h"

pailSensor::pailSensor(){}

void pailSensor::init(){
  pinMode(PAIL_SNS_PIN, INPUT);
}

void pailSensor::update(){
  uint32_t currentReading = this->readSensor();
  if(currentReading > detectionThreshold) this->state = true;
  else this->state = false;
}

void pailSensor::setDetectionThreshold(uint32_t newThreshold){
  if(newThreshold > 4085) return;
  detectionThreshold = newThreshold;
}

bool pailSensor::getState(){
  return this->state;
}

uint32_t pailSensor::readSensor(){
  return analogRead(PAIL_SNS_PIN);
}
