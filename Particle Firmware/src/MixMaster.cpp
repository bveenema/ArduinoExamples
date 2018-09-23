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

  static uint32_t timeToMix = 0;
  static uint32_t timeStartedMixing = 0;

  if(mixerState == Idle){
    idlePumps();
    if(_changeState == true) { _changeState = false; mixerState = MixingCalculations; }
  }else if(mixerState == MixingCalculations){
    resinPumpSpeed = calculatePumpSpeed(settings.flowRate[selector], settings.ratioResin[selector], settings.ratioHardener[selector], settings.stepsPerMlResin);
    hardenerPumpSpeed = calculatePumpSpeed(settings.flowRate[selector], settings.ratioHardener[selector], settings.ratioResin[selector], settings.stepsPerMlHardener);
    timeToMix = calculateTimeForVolume(settings.volume[selector], settings.flowRate[selector]);
    timeStartedMixing = millis();
    digitalWrite(RESIN_PUMP_ENABLE_PIN, LOW); // Enable Resin Pump
    digitalWrite(HARDENER_PUMP_ENABLE_PIN, LOW); // Enable Hardener Pump
    mixerState = Mixing;
  }else if(mixerState == Mixing){
    runPumps();
    if(_changeState == true || (millis() - timeStartedMixing > timeToMix)){
      _changeState = false;
      if(settings.autoReverseResin[selector] > 0 || settings.autoReverseHardener[selector] > 0) mixerState = StartAutoReverse;
      else mixerState = Idle;
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
      mixerState = Idle;
    }
  }else if(mixerState == Cleaning){
    this.updateCleaning();
    if(_changeState == true || (millis()-timeStartedCleaning > cleaningTime)){
      mixerState = Idle;
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
  const uint32_t cycleTime = 60000; // Amount of time between starting pulses - 1 minute

  switch(CleaningState){
    case InitCleaning:
      const uint32_t cleaningFlowRate = 200; // ml/min
      const uint32_t cleaningVolume = 200; // ml
      const uint32_t cleaningRatioResin = 100;
      const uint32_t cleaningRatioHardener = 100;

      // We don't know current pump state, so assume it's running and stop it
      idlePumps();

      // Configure the pumpss for cleaning
      resinPumpSpeed = calculatePumpSpeed(cleaningFlowRate, cleaningRatioResin, cleaningRatioHardener, settings.stepsPerMlResin);
      hardenerPumpSpeed = calculatePumpSpeed(cleaningFlowRate, cleaningRatioHardener, cleaningRatioResin, settings.stepsPerMlHardener);

      // Calculate Cleaning Pulse and Idle times
      cleaningPulseTime = calculateTimeForVolume(cleaningVolume, cleaningFlowRate);
      if(cleaningPulseTime < cycleTime){
        cleaningIdleTime = cycleTime - cleaningPulseTime;
      } else {
        cleaningIdleTime = 0;
      }

      timeStateStarted = millis();
      digitalWrite(RESIN_PUMP_ENABLE_PIN, LOW); // Enable Resin Pump
      digitalWrite(HARDENER_PUMP_ENABLE_PIN, LOW); // Enable Hardener Pump

      CleaningState = PulseOn;

      // No break, fall through to pulse pumps on immediately

    case PulseOn:
      runPumps();
      if(millis()-cleaningPulseTime > timeStateStarted){
        timeStateStarted = millis();
        CleaningState = IdleCleaning;
      }

      break;

    case IdleCleaning:
      idlePumps();
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
