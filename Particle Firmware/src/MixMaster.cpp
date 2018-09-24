#include "MixMaster.h"

// Utility Functions
uint32_t calculatePumpSpeed(uint32_t flowRate, uint32_t thisPumpRatio, uint32_t otherPumpRatio, uint32_t stepsPerMl){
  if(thisPumpRatio == 0 && otherPumpRatio == 0) return 0; // prevent divide by 0 error
  return flowRate*stepsPerMl*thisPumpRatio/(thisPumpRatio+otherPumpRatio)/60;
}

uint32_t calculateTimeForVolume(uint32_t volume, uint16_t flowRate){
  if(volume > 7158278) return 0; // Largest value before variable overflow
  if(flowRate == 0) return 0; // prevent divide by 0 error
  uint32_t time = volume*600/flowRate;
  return time * 100;
}

uint32_t calculateAutoReverseSteps(uint32_t thisPumpRatio, uint32_t otherPumpRatio){
  if(thisPumpRatio == 0 && otherPumpRatio == 0) return 0; // prevent divide by 0 error
  return (settings.autoReverseSteps*100)/(thisPumpRatio+otherPumpRatio)*thisPumpRatio/100;
}

// Class Functions
mixMaster::mixMaster() :
  ResinPump(AccelStepper::DRIVER, RESIN_PUMP_STEP_PIN, RESIN_PUMP_DIR_PIN),
  HardenerPump(AccelStepper::DRIVER, HARDENER_PUMP_STEP_PIN, HARDENER_PUMP_DIR_PIN)
  {}

void mixMaster::init(){
  PressureManager.init();
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

bool mixMaster::update(bool _changeState){

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
        _changeState = false;
      } else {
        timeToMix = this->prepForMixing(settings.flushVolume, settings.flowRate[selector]);
        isFlushing = true;
      }
      timeStartedMixing = millis();
      mixerState = Charging;
    }
  }else if(mixerState == Charging){
    PressureManager.setChargingState(true);
    if(PressureManager.isCharged()) mixerState = Mixing;
  }else if(mixerState == Mixing){
    if(this->runPumpsWithErrorCheck()) mixerState = StartIdle;
    if(_changeState == true || (millis() - timeStartedMixing > timeToMix)){
      // don't reset _changeState when flushing so button won't be "ignored" while flushing
      if(!isFlushing) _changeState = false;
      if(settings.autoReverseSteps > 0) mixerState = StartAutoReverse;
      else mixerState = StartIdle;
    }
  }else if(mixerState == StartAutoReverse){
    ResinPump.setMaxSpeed(autoReverseSpeed);
    HardenerPump.setMaxSpeed(autoReverseSpeed);
    ResinPump.setCurrentPosition(0);
    HardenerPump.setCurrentPosition(0);
    ResinPump.moveTo(-calculateAutoReverseSteps(settings.ratioResin[selector], settings.ratioHardener[selector]));
    HardenerPump.moveTo(-calculateAutoReverseSteps(settings.ratioHardener[selector], settings.ratioResin[selector]));
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
      _changeState = false;
    }
  }

  return _changeState;
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

void mixMaster::startCleaning(){
  mixerState = Cleaning;
  CleaningState = InitCleaning;
  timeStartedCleaning = millis();
}

void mixMaster::updateCleaning(){
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
      if(this->runPumpsWithErrorCheck()) mixerState = StartIdle;
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

uint32_t mixMaster::prepForMixing(uint32_t volume, uint32_t flowRate){
  resinPumpSpeed = calculatePumpSpeed(flowRate, settings.ratioResin[selector], settings.ratioHardener[selector], settings.stepsPerMlResin);
  hardenerPumpSpeed = calculatePumpSpeed(flowRate, settings.ratioHardener[selector], settings.ratioResin[selector], settings.stepsPerMlHardener);
  ResinPump.setMaxSpeed(ultimateMaxSpeed);
  HardenerPump.setMaxSpeed(ultimateMaxSpeed);
  ResinPump.setSpeed(resinPumpSpeed);
  HardenerPump.setSpeed(hardenerPumpSpeed);
  digitalWrite(RESIN_PUMP_ENABLE_PIN, LOW); // Enable Resin Pump
  digitalWrite(HARDENER_PUMP_ENABLE_PIN, LOW); // Enable Hardener Pump
  return calculateTimeForVolume(volume, flowRate);
}

void mixMaster::idlePumps(){
  PressureManager.setChargingState(false);
  ResinPump.setSpeed(0);
  HardenerPump.setSpeed(0);
  digitalWrite(RESIN_PUMP_ENABLE_PIN, HIGH); // Disable Resin Pump
  digitalWrite(HARDENER_PUMP_ENABLE_PIN, HIGH); // Disable Hardener Pump
}

void mixMaster::runPumps(){
  ResinPump.runSpeed();
  HardenerPump.runSpeed();
}

bool mixMaster::runPumpsWithErrorCheck(){
  this->runPumps();
  if(digitalRead(RESIN_PUMP_ASSERT_PIN) || digitalRead(HARDENER_PUMP_ASSERT_PIN)){
    strncpy("Pump Error",currentError,30);
    return true;
  }
  return false;
}
