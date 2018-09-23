#include "MixMaster.h"

// Utility Functions
uint32_t calculatePumpSpeed(uint16_t flowRate, uint16_t thisPumpRatio, uint16_t otherPumpRatio, uint16_t stepsPerMl){
  if(thisPumpRatio == 0 && otherPumpRatio == 0) return 0; // prevent divide by 0 error
  uint32_t pumpSpeed = flowRate*stepsPerMl*thisPumpRatio/(thisPumpRatio+otherPumpRatio)/60;
  return pumpSpeed;
}

uint32_t calculateTimeForVolume(uint32_t volume, uint16_t flowRate){
  if(volume > 7158278) return 0; // Largest value before variable overflow
  if(flowRate == 0) return 0; // prevent divide by 0 error
  uint32_t time = volume*600/flowRate;
  return time * 100;
}

// Class Functions
MixMaster::MixMaster() :
  ResinPump(AccelStepper::DRIVER, RESIN_PUMP_STEP_PIN, RESIN_PUMP_DIR_PIN),
  HardenerPump(AccelStepper::DRIVER, HARDENER_PUMP_STEP_PIN, HARDENER_PUMP_DIR_PIN)
  {}

void MixMaster::init(){
  pinMode(RESIN_PUMP_ENABLE_PIN, OUTPUT);
  pinMode(RESIN_PUMP_STEP_PIN, OUTPUT);
  pinMode(RESIN_PUMP_DIR_PIN, OUTPUT);
  pinMode(RESIN_PUMP_ASSERT_PIN, INPUT_PULLDOWN);

  pinMode(HARDENER_PUMP_ENABLE_PIN, OUTPUT);
  pinMode(HARDENER_PUMP_STEP_PIN, OUTPUT);
  pinMode(HARDENER_PUMP_DIR_PIN, OUTPUT);
  pinMode(HARDENER_PUMP_ASSERT_PIN, INPUT_PULLDOWN);

  digitalWrite(RESIN_PUMP_ENABLE_PIN, LOW); // Enable Resin Pump
  digitalWrite(HARDENER_PUMP_ENABLE_PIN, LOW); // Enable Hardener Pump

  ResinPump.setAcceleration(100000);
  HardenerPump.setAcceleration(100000);

  ResinPump.setPinsInverted(0,0,1);
  HardenerPump.setPinsInverted(0,0,1);

  ResinPump.setMaxSpeed(ultimateMaxSpeed);
  HardenerPump.setMaxSpeed(ultimateMaxSpeed);
}

bool MixMaster::update(bool _changeState){

  PressureManager.update();

  static uint32_t timeToMix = 0;
  static uint32_t timeStartedMixing = 0;
  static uint32_t timeStartedIdling = 0;
  static bool isFlushing = false;

  if(mixerState == StartIdle){
    timeStartedIdling = millis();
    mixerState = Idle;
  }else if(mixerState == Idle){
    this->idlePumps();
    if(_changeState || (millis() - timeStartedIdling > TIME_BETWEEN_FLUSHES)) {
      if(_changeState){
        timeToMix = this->prepForMixing(settings.volume[selector], settings.flowRate[selector]);
        isFlushing = false;
      } else {
        timeToMix = this->prepForMixing(FLUSH_VOLUME, settings.flowRate[selector]);
        isFlushing = true;
      }
      timeStartedMixing = millis();
      mixerState = Charging;
    }
  }else if(mixerState == Charging){
    PressureManager.setChargingState(true);
    if(PressureManager.isCharged()) mixerState = Mixing;
  }else if(mixerState == Mixing){
    this->runPumps();
    if(_changeState == true || (millis() - timeStartedMixing > timeToMix)){
      // don't reset _changeState when flushing so button won't be "ignored" while flushing
      if(!isFlushing) _changeState = false;
      if(settings.autoReverseResin[selector] > 0 || settings.autoReverseHardener[selector] > 0) mixerState = StartAutoReverse;
      else mixerState = StartIdle;
    }
  }else if(mixerState == StartAutoReverse){
    ResinPump.setMaxSpeed(autoReverseSpeed);
    HardenerPump.setMaxSpeed(autoReverseSpeed);
    ResinPump.setCurrentPosition(0);
    HardenerPump.setCurrentPosition(0);
    ResinPump.moveTo(-settings.autoReverseResin[selector]);
    HardenerPump.moveTo(-settings.autoReverseHardener[selector]);
    mixerState = AutoReversing;
  }else if(mixerState == AutoReversing){ // AutoReversing
    ResinPump.run();
    HardenerPump.run();
    if(!ResinPump.isRunning() && !HardenerPump.isRunning()){
      mixerState = StartIdle;
    }
  }else if(mixerState == Cleaning){
    this->updateCleaning();
    if(_changeState == true || (millis()-timeStartedCleaning > CLEANING_CYCLE_DURATION)){
      mixerState = StartIdle;
    }
  }

  return _changeState;
}

MixerState MixMaster::getState(){
  return mixerState;
}

uint32_t MixMaster::getPumpSpeed(MixerChannel channel){
  if(channel == Resin){
    return resinPumpSpeed;
  }
  return hardenerPumpSpeed;
}

void MixMaster::startCleaning(){
  mixerState = Cleaning;
  CleaningState = InitCleaning;
  timeStartedCleaning = millis();
}

void MixMaster::updateCleaning(){
  static uint32_t cleaningPulseTime;
  static uint32_t cleaningIdleTime;
  static uint32_t timeStateStarted;

  switch(CleaningState){
    case InitCleaning: {

      // We don't know current pump state, so assume it's running and stop it
      this->idlePumps();

      // Calculate Cleaning Pulse and Idle times, prep for mixing
      cleaningPulseTime = prepForMixing(CLEANING_VOLUME_PER_PULSE, CLEANING_FLOW_RATE);
      if(cleaningPulseTime < CLEANING_PULSE_INTERVAL){
        cleaningIdleTime = CLEANING_PULSE_INTERVAL - cleaningPulseTime;
      } else {
        cleaningIdleTime = 0;
      }

      timeStateStarted = millis();
      digitalWrite(RESIN_PUMP_ENABLE_PIN, LOW); // Enable Resin Pump
      digitalWrite(HARDENER_PUMP_ENABLE_PIN, LOW); // Enable Hardener Pump

      CleaningState = PulseOn;
    }
    // No break, fall through to pulse pumps on immediately
    case PulseOn:
      this->runPumps();
      if(millis()-cleaningPulseTime > timeStateStarted){
        timeStateStarted = millis();
        CleaningState = IdleCleaning;
      }

      break;

    case IdleCleaning:
      this->idlePumps();
      if(millis()-cleaningIdleTime > timeStateStarted){
        timeStateStarted = millis();
        CleaningState = PulseOn;
      }

      break;

    default:
      CleaningState = InitCleaning;
      break;
  }
}

uint32_t MixMaster::prepForMixing(uint32_t volume, uint32_t flowRate){
  resinPumpSpeed = calculatePumpSpeed(flowRate, settings.ratioResin[selector], settings.ratioHardener[selector], settings.stepsPerMlResin);
  hardenerPumpSpeed = calculatePumpSpeed(flowRate, settings.ratioHardener[selector], settings.ratioResin[selector], settings.stepsPerMlHardener);
  digitalWrite(RESIN_PUMP_ENABLE_PIN, LOW); // Enable Resin Pump
  digitalWrite(HARDENER_PUMP_ENABLE_PIN, LOW); // Enable Hardener Pump
  return calculateTimeForVolume(volume, flowRate);
}

void MixMaster::idlePumps(){
  PressureManager.setChargingState(false);
  ResinPump.setSpeed(0);
  HardenerPump.setSpeed(0);
  digitalWrite(RESIN_PUMP_ENABLE_PIN, HIGH); // Disable Resin Pump
  digitalWrite(HARDENER_PUMP_ENABLE_PIN, HIGH); // Disable Hardener Pump
}

void MixMaster::runPumps(){
  ResinPump.setMaxSpeed(ultimateMaxSpeed);
  HardenerPump.setMaxSpeed(ultimateMaxSpeed);
  ResinPump.setSpeed(resinPumpSpeed);
  HardenerPump.setSpeed(hardenerPumpSpeed);
  ResinPump.runSpeed();
  HardenerPump.runSpeed();
}
