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
  static bool keepOpen = false;

  if(mixerState == StartIdle){
    timeStartedIdling = millis();
    mixerState = Idle;
  }else if(mixerState == Idle){
    this->idlePumps();
    if(_changeState || (millis() - timeStartedIdling > TIME_BETWEEN_KEEP_OPEN_CYCLES)) {
      Serial.println("Changing State from Idle");
      if(_changeState){
        timeToMix = this->prepForMixing(settings.volume[selector], settings.flowRate[selector]);
        keepOpen = false;
        _changeState = false;
      } else {
        timeToMix = this->prepForMixing(settings.keepOpenVolume, settings.flowRate[selector]);
        keepOpen = true;
      }
      timeStartedMixing = millis();
      mixerState = Charging;
    }
  }else if(mixerState == Charging){
    Serial.println("Charging");
    PressureManager.setChargingState(true);
    if(PressureManager.isCharged()) mixerState = Mixing;
  }else if(mixerState == Mixing){
    Serial.println("Mixing");
    if(this->runPumpsWithErrorCheck()) mixerState = StartIdle;
    if(_changeState == true || (millis() - timeStartedMixing > timeToMix)){
      // don't reset _changeState when keep open so button won't be "ignored" while keep open
      if(!keepOpen) _changeState = false;
      if(settings.autoReverseSteps > 0) mixerState = StartAutoReverse;
      else mixerState = StartIdle;
    }
  }else if(mixerState == StartAutoReverse){
    Serial.println("StartAutoReverse");
    ResinPump.setMaxSpeed(autoReverseSpeed);
    HardenerPump.setMaxSpeed(autoReverseSpeed);
    ResinPump.setCurrentPosition(0);
    HardenerPump.setCurrentPosition(0);
    ResinPump.moveTo(-calculateAutoReverseSteps(settings.ratioResin[selector], settings.ratioHardener[selector]));
    HardenerPump.moveTo(-calculateAutoReverseSteps(settings.ratioHardener[selector], settings.ratioResin[selector]));
    mixerState = AutoReversing;
  }else if(mixerState == AutoReversing){ // AutoReversing
    Serial.println("AutoReversing");
    ResinPump.run();
    HardenerPump.run();
    if(!ResinPump.isRunning() && !HardenerPump.isRunning()){
      mixerState = StartIdle;
    }
  }else if(mixerState == Flushing){
    Serial.println("Flushing");
    this->updateFlushing();
    if(_changeState == true || (millis()-timeStartedFlushing > FLUSH_CYCLE_DURATION)){
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

void mixMaster::startFlush(){
  mixerState = Flushing;
  FlushingState = InitFlush;
  timeStartedFlushing = millis();
}

void mixMaster::updateFlushing(){
  static uint32_t flushPulseTime;
  static uint32_t flushIdleTime;
  static uint32_t timeStateStarted;

  switch(FlushingState){
    case InitFlush: {

      // We don't know current pump state, so assume it's running and stop it
      this->idlePumps();

      // Calculate Flush Pulse and Idle times, prep for mixing
      flushPulseTime = prepForMixing(FLUSH_VOLUME_PER_PULSE, FLUSH_FLOW_RATE);
      if(flushPulseTime < FLUSH_PULSE_INTERVAL){
        flushIdleTime = FLUSH_PULSE_INTERVAL - flushPulseTime;
      } else {
        flushIdleTime = 0;
      }

      timeStateStarted = millis();
      digitalWrite(RESIN_PUMP_ENABLE_PIN, LOW); // Enable Resin Pump
      digitalWrite(HARDENER_PUMP_ENABLE_PIN, LOW); // Enable Hardener Pump

      FlushingState = PulseOn;
    }
    // No break, fall through to pulse pumps on immediately
    case PulseOn:
      if(this->runPumpsWithErrorCheck()) mixerState = StartIdle;
      if(millis()-flushPulseTime > timeStateStarted){
        timeStateStarted = millis();
        FlushingState = IdleFlushing;
      }

      break;

    case IdleFlushing:
      this->idlePumps();
      if(millis()-flushIdleTime > timeStateStarted){
        timeStateStarted = millis();
        FlushingState = PulseOn;
      }

      break;

    default:
      FlushingState = InitFlush;
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
    Serial.println("Error Detected");
    strncpy("Pump Error",currentError,30);
    return true;
  }
  return false;
}
