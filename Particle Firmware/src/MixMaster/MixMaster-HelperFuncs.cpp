#include "MixMaster.h"

// Utility Functions
uint32_t mixMaster::RPMtoStepsPerSecond(uint32_t RPM){
  return RPM*STEPS_PER_REV/60;
}

uint32_t mixMaster::totalVolumeToSteps(uint32_t volume, uint32_t stepsPerMl, uint32_t thisRatio, uint32_t otherRatio){
  uint32_t totalRatio = thisRatio + otherRatio;
  if(totalRatio == 0) return 0; // prevent divide by 0 error
  return volume*stepsPerMl*thisRatio/totalRatio; // # of steps
}

uint32_t mixMaster::getSpeedFromRPM(uint32_t RPM, uint32_t thisStepsPerMl, uint32_t otherStepsPerMl, uint32_t thisRatio, uint32_t otherRatio){
  uint32_t totalRatio = thisRatio + otherRatio;
  uint32_t totalStepsPerMl = thisStepsPerMl + otherStepsPerMl;
  if(totalRatio == 0 || totalStepsPerMl == 0) return 0; // prevent divide by 0 error
  return RPM*STEPS_PER_REV*thisStepsPerMl*thisRatio/60/totalStepsPerMl/totalRatio; //steps/ml
}

uint32_t mixMaster::stepsToMl(uint32_t steps, uint32_t stepsPerMl){
  return steps/stepsPerMl;
}

uint32_t mixMaster::calculatePumpSpeed(uint32_t flowRate, uint32_t thisPumpRatio, uint32_t otherPumpRatio, uint32_t stepsPerMl){
  if(thisPumpRatio == 0 && otherPumpRatio == 0) return 0; // prevent divide by 0 error
  return flowRate*stepsPerMl*thisPumpRatio/(thisPumpRatio+otherPumpRatio)/60;
}

uint32_t mixMaster::calculateTimeForVolume(uint32_t volume, uint16_t flowRate){
  if(volume > 7158278) return 0; // Largest value before variable overflow
  if(flowRate == 0) return 0; // prevent divide by 0 error
  uint32_t time = volume*600/flowRate;
  return time * 100;
}

uint32_t mixMaster::calculateAutoReverseSteps(uint32_t thisPumpRatio, uint32_t otherPumpRatio){
  if(thisPumpRatio == 0 && otherPumpRatio == 0) return 0; // prevent divide by 0 error
  return (settings.autoReverseSteps*100)/(thisPumpRatio+otherPumpRatio)*thisPumpRatio/100;
}

uint32_t mixMaster::prepForMixing(uint32_t volume, uint32_t flowRate, uint32_t ratioResin, uint32_t ratioHardener){
  resinPumpSpeed = calculatePumpSpeed(flowRate, ratioResin, ratioHardener, settings.stepsPerMlResin);
  hardenerPumpSpeed = calculatePumpSpeed(flowRate, ratioHardener, ratioResin, settings.stepsPerMlHardener);
  ResinPump.setMaxSpeed(ultimateMaxSpeed);
  HardenerPump.setMaxSpeed(ultimateMaxSpeed);
  ResinPump.setSpeed(resinPumpSpeed);
  HardenerPump.setSpeed(hardenerPumpSpeed);
  enablePumps();


  return calculateTimeForVolume(volume, flowRate);
}

void mixMaster::idlePumps(){
  // Update Pump Handler maybe running, so kill it
  pumpUpdater.end();

  ResinPump.setSpeed(0);
  HardenerPump.setSpeed(0);

  disablePumps();
}

void mixMaster::enablePumps(){
  if(_pumpsEnabledState == false){
    IOExp.digitalWrite(RESIN_ENABLE_IOEXP_PIN, LOW); // Enable Resin Pump
    IOExp.digitalWrite(HARDENER_ENABLE_IOEXP_PIN, LOW); // Enable Hardener Pump
    _pumpsEnabledState = true;
  }
}

void mixMaster::disablePumps(){
  if(_pumpsEnabledState == true){
    IOExp.digitalWrite(RESIN_ENABLE_IOEXP_PIN, HIGH); // Disable Resin Pump
    IOExp.digitalWrite(HARDENER_ENABLE_IOEXP_PIN, HIGH); // Disable Hardener Pump
    _pumpsEnabledState = false;
  }
}

void mixMaster::runPumps(){
  ResinPump.runSpeed();
  HardenerPump.runSpeed();
}

bool mixMaster::runPumpsWithErrorCheck(){
  this->runPumps();
  return this->checkPumpErrors();
}


bool mixMaster::checkPumpErrors(){
  bool returnVal = false;

  static uint32_t accumulatePumpError = 0;
  static uint32_t accumulatePailError = 0;
  // Check pumps for error, return true if error for more than 50 ms
  static uint32_t lastPumpErrorRead = 0;
  if(millis() - lastPumpErrorRead > 10){
    if(!IOExp.digitalRead(RESIN_HLFB_IOEXP_PIN) || !IOExp.digitalRead(HARDENER_HLFB_IOEXP_PIN)){
      accumulatePumpError += 1;
      if(accumulatePumpError > 5){
        strncpy(currentError, "Pump Error",30);
        returnVal = true;
      }
    } else {
      accumulatePumpError = 0;
    }
  }

  // Check Output Pail error, return true if output pail not detected
  if(!PailSensor.getState()){
    accumulatePailError += 1;
    if(accumulatePailError > 10000){
      strncpy(currentError, "Pail Error", 30);
      returnVal = true;
    }
  } else {
    accumulatePailError = 0;
  }

  // if not yet primed and there's an error, reset numConsecutivePrimes
  if(!isPrimed && returnVal) {
    Serial.printlnf("numConsecutivePrimes:%d",numConsecutivePrimes);
    numConsecutivePrimes = 0;
  }
  return returnVal;
}
