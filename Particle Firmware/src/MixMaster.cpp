#include "MixMaster.h"

// Utility Functions
uint32_t calculateMotorSpeed(uint16_t flowRate, uint16_t thisMotorRatio, uint16_t otherMotorRatio, uint16_t stepsPerMl){
  if(thisMotorRatio == 0 && otherMotorRatio == 0) return 0; // prevent divide by 0 error
  uint32_t motorSpeed = flowRate*stepsPerMl*thisMotorRatio/(thisMotorRatio+otherMotorRatio)/60;
  return motorSpeed;
}

uint32_t calculateTimeForVolume(uint32_t volume, uint16_t flowRate){
  if(volume > 7158278) return 0; // Largest value before variable overflow
  if(flowRate == 0) return 0; // prevent divide by 0 error
  uint32_t time = volume*600/flowRate;
  return time * 100;
}

void idlePumps(){
  motorResin.setSpeed(0);
  motorHardener.setSpeed(0);
  digitalWrite(MOTOR_RESIN_ENABLE_PIN, HIGH); // Disable Resin Motor
  digitalWrite(MOTOR_HARDENER_ENABLE_PIN, HIGH); // Disable Hardener Motor
}

void runPumps(){
  motorResin.setMaxSpeed(ultimateMaxSpeed);
  motorHardener.setMaxSpeed(ultimateMaxSpeed);
  motorResin.setSpeed(motorSpeedResin);
  motorHardener.setSpeed(motorSpeedHardener);
  motorResin.runSpeed();
  motorHardener.runSpeed();
}

// Class Functions
MixMaster::MixMaster() :
  motorResin(AccelStepper::DRIVER, MOTOR_RESIN_STEP_PIN, MOTOR_RESIN_DIR_PIN),
  motorHardener(AccelStepper::DRIVER, MOTOR_HARDENER_STEP_PIN, MOTOR_HARDENER_DIR_PIN)
  {}

void MixMaster::init(){
  pinMode(MOTOR_RESIN_ENABLE_PIN, OUTPUT);
  pinMode(MOTOR_RESIN_STEP_PIN, OUTPUT);
  pinMode(MOTOR_RESIN_DIR_PIN, OUTPUT);
  pinMode(MOTOR_RESIN_ASSERT_PIN, INPUT_PULLDOWN);

  pinMode(MOTOR_HARDENER_ENABLE_PIN, OUTPUT);
  pinMode(MOTOR_HARDENER_STEP_PIN, OUTPUT);
  pinMode(MOTOR_HARDENER_DIR_PIN, OUTPUT);
  pinMode(MOTOR_HARDENER_ASSERT_PIN, INPUT_PULLDOWN);

  digitalWrite(MOTOR_RESIN_ENABLE_PIN, LOW); // Enable Resin Motor
  digitalWrite(MOTOR_HARDENER_ENABLE_PIN, LOW); // Enable Hardener Motor

  motorResin.setAcceleration(100000);
  motorHardener.setAcceleration(100000);

  motorResin.setPinsInverted(0,0,1);
  motorHardener.setPinsInverted(0,0,1);

  motorResin.setMaxSpeed(ultimateMaxSpeed);
  motorHardener.setMaxSpeed(ultimateMaxSpeed);
}

bool MixMaster::update(bool _changeState){

  static uint32_t timeToMix = 0;
  static uint32_t timeStartedMixing = 0;

  if(mixerState == Idle){
    idlePumps();
    if(_changeState == true) { _changeState = false; mixerState = MixingCalculations; }
  }else if(mixerState == MixingCalculations){
    motorSpeedResin = calculateMotorSpeed(settings.flowRate[selector], settings.ratioResin[selector], settings.ratioHardener[selector], settings.stepsPerMlResin);
    motorSpeedHardener = calculateMotorSpeed(settings.flowRate[selector], settings.ratioHardener[selector], settings.ratioResin[selector], settings.stepsPerMlHardener);
    timeToMix = calculateTimeForVolume(settings.volume[selector], settings.flowRate[selector]);
    timeStartedMixing = millis();
    digitalWrite(MOTOR_RESIN_ENABLE_PIN, LOW); // Enable Resin Motor
    digitalWrite(MOTOR_HARDENER_ENABLE_PIN, LOW); // Enable Hardener Motor
    mixerState = Mixing;
  }else if(mixerState == Mixing){
    runPumps();
    if(_changeState == true || (millis() - timeStartedMixing > timeToMix)){
      _changeState = false;
      if(settings.autoReverseResin[selector] > 0 || settings.autoReverseHardener[selector] > 0) mixerState = StartAutoReverse;
      else mixerState = Idle;
    }
  }else if(mixerState == StartAutoReverse){
    motorResin.setMaxSpeed(autoReverseSpeed);
    motorHardener.setMaxSpeed(autoReverseSpeed);
    motorResin.setCurrentPosition(0);
    motorHardener.setCurrentPosition(0);
    motorResin.moveTo(-settings.autoReverseResin[selector]);
    motorHardener.moveTo(-settings.autoReverseHardener[selector]);
    mixerState = AutoReversing;
  }else if(mixerState == AutoReversing){ // AutoReversing
    motorResin.run();
    motorHardener.run();
    if(!motorResin.isRunning() && !motorHardener.isRunning()){
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

uint32_t MixMaster::getMotorSpeed(MixerChannel channel){
  if(channel == Resin){
    return motorSpeedResin;
  }
  return motorSpeedHardener;
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

      // We don't know current motor state, so assume it's running and stop it
      idlePumps();

      // Configure the motors for cleaning
      motorSpeedResin = calculateMotorSpeed(cleaningFlowRate, cleaningRatioResin, cleaningRatioHardener, settings.stepsPerMlResin);
      motorSpeedHardener = calculateMotorSpeed(cleaningFlowRate, cleaningRatioHardener, cleaningRatioResin, settings.stepsPerMlHardener);

      // Calculate Cleaning Pulse and Idle times
      cleaningPulseTime = calculateTimeForVolume(cleaningVolume, cleaningFlowRate);
      if(cleaningPulseTime < cycleTime){
        cleaningIdleTime = cycleTime - cleaningPulseTime;
      } else {
        cleaningIdleTime = 0;
      }

      timeStateStarted = millis();
      digitalWrite(MOTOR_RESIN_ENABLE_PIN, LOW); // Enable Resin Motor
      digitalWrite(MOTOR_HARDENER_ENABLE_PIN, LOW); // Enable Hardener Motor

      CleaningState = PulseOn;

      // No break, fall through to pulse motor on immediately

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
