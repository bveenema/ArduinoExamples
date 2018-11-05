#include "MixMaster.h"

mixMaster MixMaster;

mixMaster::mixMaster() :
  ResinPump(AccelStepper::DRIVER, RESIN_INPUT_STEP_PIN, RESIN_INPUT_DIR_PIN),
  HardenerPump(AccelStepper::DRIVER, HARDENER_INPUT_STEP_PIN, HARDENER_INPUT_DIR_PIN)
  {}

void mixMaster::init(){
  IOExp.pinMode(RESIN_ENABLE_IOEXP_PIN, OUTPUT);
  pinMode(RESIN_INPUT_STEP_PIN, OUTPUT);
  pinMode(RESIN_INPUT_DIR_PIN, OUTPUT);
  IOExp.pinMode(RESIN_HLFB_IOEXP_PIN, INPUT);

  IOExp.pinMode(HARDENER_ENABLE_IOEXP_PIN, OUTPUT);
  pinMode(HARDENER_INPUT_STEP_PIN, OUTPUT);
  pinMode(HARDENER_INPUT_DIR_PIN, OUTPUT);
  IOExp.pinMode(HARDENER_HLFB_IOEXP_PIN, INPUT);

  IOExp.digitalWrite(RESIN_ENABLE_IOEXP_PIN, LOW); // Enable Resin Pump
  IOExp.digitalWrite(HARDENER_ENABLE_IOEXP_PIN, LOW); // Enable Hardener Pump

  ResinPump.setAcceleration(100000);
  HardenerPump.setAcceleration(100000);

  ResinPump.setPinsInverted(0,0,1);
  HardenerPump.setPinsInverted(0,0,1);

  ResinPump.setMaxSpeed(ultimateMaxSpeed);
  HardenerPump.setMaxSpeed(ultimateMaxSpeed);
}

MixerState mixMaster::getState(){
  return mixerState;
}

uint32_t mixMaster::getPumpSpeed(MixerChannel channel){
  if(channel == Resin){
    return resinPumpSpeed;
  }
  return hardenerPumpSpeed;
}
