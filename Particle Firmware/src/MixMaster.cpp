#include "MixMaster.h"

mixMaster MixMaster;

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

// Pump handler
void updatePumps(){
  MixMaster.runPumps();
}

// Class Functions
mixMaster::mixMaster() :
  ResinPump(AccelStepper::DRIVER, RESIN_PUMP_STEP_PIN, RESIN_PUMP_DIR_PIN),
  HardenerPump(AccelStepper::DRIVER, HARDENER_PUMP_STEP_PIN, HARDENER_PUMP_DIR_PIN)
  {}

void mixMaster::init(){
  PressureManager.init(settings.onPressure, settings.offPressure);
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
  static uint32_t timeToMix = 0;
  static uint32_t timeStartedCharging = 0;
  static uint32_t mixingTimer = 0;
  static uint32_t timeStartedIdling = 0;
  static uint32_t previousMillis = 0;
  static bool keepOpen = false;
  static bool prime = false;
  static bool wasCharging = false;
  static uint32_t timeStartedChargingWhileMixing = 0;
  static uint32_t timeEndedCharging = 0;

  if(mixerState == START_IDLE){ // 0
    if(numConsecutivePrimes >= settings.minPrimes) isPrimed = true;
    timeStartedIdling = millis();
    mixerState = IDLE;
  }else if(mixerState == IDLE){ // 1
    this->idlePumps();
    PressureManager.update(false);
    if(_changeState || (millis() - timeStartedIdling > TIME_BETWEEN_KEEP_OPEN_CYCLES)) {
      #ifdef PAIL_SENSOR_ENABLED
      // If no Pail in position, prevent keep open or mixing
      if(!PailSensor.getState()){
        mixerState = START_IDLE;
        _changeState = false;
        return _changeState;
      }
      #endif
      if(_changeState){
        if(!isPrimed){
          Serial.printlnf("Priming:%d",numConsecutivePrimes);
          prime = true;
        } else{
          prime = false;
        }
        keepOpen = false;
        _changeState = false;
      } else { // keep open
        // prevent keepOpen if not primed
        if(!isPrimed) {
          mixerState = START_IDLE;
          return _changeState;
        }
        keepOpen = true;
        prime = false;

      }
      mixerState = CHARGING;
      timeStartedCharging = millis();
    }
  }else if(mixerState == CHARGING){ // 2
    PressureManager.setChargingState(true);
    PressureManager.update(true);
    if(PressureManager.isCharged() || (millis()-timeStartedCharging > settings.chargeTimeout)) {
      if(prime) timeToMix = this->prepForMixing(settings.primeVolume, settings.flowRate[selector]);
      else if(keepOpen) timeToMix = this->prepForMixing(settings.keepOpenVolume, settings.flowRate[selector]);
      else timeToMix = this->prepForMixing(settings.volume[selector], settings.flowRate[selector]);

      Serial.printlnf("Time to Mix:%d",timeToMix);
      mixingTimer = 0;
      previousMillis = millis();
      wasCharging = true;
      timeEndedCharging = millis();
      mixerState = MIXING;
    }
    if(_changeState == true){
      // don't reset _changeState when keep open so button won't be "ignored" while keep open
      if(!keepOpen) _changeState = false;
      mixerState = START_IDLE;
    }
  }else if(mixerState == MIXING){ // 3
    // PressureManager.update returns true when the pump needs to run
    static bool allowCharging = false;

      // pause pumping
      pumpUpdater.end();

      if(millis() - timeStartedChargingWhileMixing > settings.chargeDelay){
        allowCharging = true; // allow charging
        wasCharging = true;// set wasCharging
        timeEndedChargingWhileMixing = millis();

        // check for pressure manager to stop requesting charging
        if(!PressureManager.update(allowCharging)){
          lastChargingTime = millis();
        }
      }
    } else if(wasCharging){
      allowCharging = false;
      if(millis() - timeEndedChargingWhileMixing > settings.chargeDelay){
        wasCharging = false;
        pumpUpdater.begin(updatePumps, 10, uSec); // resume pumping
        previousMillis = millis(); // resume timer
        lastChargingTime = millis();
      }
    } else {
      timeStartedChargingWhileMixing = millis();
    }

    // keep pressure manager updated
    PressureManager.update(allowCharging);

    // if the Pressure Manager is not charging, check the pump for errors and increment mixing timer
    if(!allowCharging && millis() - previousMillis > 0){
      mixingTimer += (millis() - previousMillis);
      previousMillis = millis();
      if(this->checkPumpErrors()) mixerState = START_IDLE;
    }

    if(_changeState == true || (mixingTimer > timeToMix)){
      // don't reset _changeState when keep open so button won't be "ignored" while keep open
      if(!keepOpen) _changeState = false;
      if(settings.autoReverseSteps > 0) mixerState = START_AUTO_REVERSE;
      else mixerState = START_IDLE;

      // kill pump interrupt handler
      pumpUpdater.end();
    }
  }else if(mixerState == START_AUTO_REVERSE){ // 4
    ResinPump.setMaxSpeed(autoReverseSpeed);
    HardenerPump.setMaxSpeed(autoReverseSpeed);
    ResinPump.setCurrentPosition(0);
    HardenerPump.setCurrentPosition(0);
    ResinPump.moveTo(-calculateAutoReverseSteps(settings.ratioResin[selector], settings.ratioHardener[selector]));
    HardenerPump.moveTo(-calculateAutoReverseSteps(settings.ratioHardener[selector], settings.ratioResin[selector]));
    mixerState = AUTO_REVERSING;
  }else if(mixerState == AUTO_REVERSING){ // 5
    ResinPump.run();
    HardenerPump.run();
    if(!ResinPump.isRunning() && !HardenerPump.isRunning()){
      // if the cycle was a prime and it got here, increment numConsecutivePrimes
      if(prime){
        numConsecutivePrimes += 1;
      }
      mixerState = START_IDLE;
    }
  }else if(mixerState == FLUSHING){ // 6
    // kill Flusing if pail no pail
    #ifdef PAIL_SENSOR_ENABLED
    if(!PailSensor.getState()){
      mixerState = START_IDLE;
      _changeState = false;
      return _changeState;
    }
    #endif
    this->updateFlushing();
    if(_changeState == true || (millis()-timeStartedFlushing > FLUSH_CYCLE_DURATION)){
      mixerState = START_IDLE;
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
  mixerState = FLUSHING;
  FlushingState = INITFLUSH;
  timeStartedFlushing = millis();
}

void mixMaster::updateFlushing(){
  static uint32_t flushPulseTime;
  static uint32_t flushIdleTime;
  static uint32_t timeStateStarted;

  switch(FlushingState){
    case INITFLUSH:
      // We don't know current pump state, so assume it's running and stop it
      this->idlePumps();

      // Calculate Flush Pulse and Idle times, prep for mixing
      flushPulseTime = prepForMixing(FLUSH_VOLUME_PER_PULSE, FLUSH_FLOW_RATE, 100, 100);
      if(flushPulseTime < FLUSH_PULSE_INTERVAL){
        flushIdleTime = FLUSH_PULSE_INTERVAL - flushPulseTime;
      } else {
        flushIdleTime = 0;
      }

      timeStateStarted = millis();
      digitalWrite(RESIN_PUMP_ENABLE_PIN, LOW); // Enable Resin Pump
      digitalWrite(HARDENER_PUMP_ENABLE_PIN, LOW); // Enable Hardener Pump

      FlushingState = PULSE_ON;

      break;

    case PULSE_ON:
      if(this->checkPumpErrors()) mixerState = START_IDLE;
      if(millis()-timeStateStarted > flushPulseTime){
        timeStateStarted = millis();
        FlushingState = IDLE_FLUSHING;
      }

      break;

    case IDLE_FLUSHING:
      this->idlePumps();
      if(millis()-timeStateStarted > flushIdleTime){
        timeStateStarted = millis();
        FlushingState = INITFLUSH;
      }

      break;

    default:
      Serial.println("Default Case");
      FlushingState = INITFLUSH;
      break;
  }
}

uint32_t mixMaster::prepForMixing(uint32_t volume, uint32_t flowRate, uint32_t ratioResin, uint32_t ratioHardener){
  resinPumpSpeed = calculatePumpSpeed(flowRate, ratioResin, ratioHardener, settings.stepsPerMlResin);
  hardenerPumpSpeed = calculatePumpSpeed(flowRate, ratioHardener, ratioResin, settings.stepsPerMlHardener);
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
  // Update Pump Handler maybe running, so kill it
  pumpUpdater.end();

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
  return this->checkPumpErrors();
}

bool mixMaster::checkPumpErrors(){
  bool returnVal = false;

  static uint32_t accumulatePumpError = 0;
  static uint32_t accumulateChargeError = 0;
  static uint32_t accumulatePailError = 0;
  // Check pumps for error, return true if more than 100 checks in a row are error
  if(!digitalRead(RESIN_PUMP_ASSERT_PIN) || !digitalRead(HARDENER_PUMP_ASSERT_PIN)){
    if(!digitalRead(RESIN_PUMP_ASSERT_PIN)) Serial.println("Error Resin Pump");
    if(!digitalRead(HARDENER_PUMP_ASSERT_PIN)) Serial.println("Error Hardener Pump");
    accumulatePumpError += 1;
    if(accumulatePumpError > 100){
      strncpy(currentError, "Pump Error",30);
      returnVal = true;
    }
  } else {
    accumulatePumpError = 0;
  }

  // Check Charging for error, return true if longer than settings.maxNoPressure are error
  // Don't check for charging error when flushing
  if(mixerState != FLUSHING && !PressureManager.isCharged()){
    static uint32_t lastNotChargedTime = 0;
    if(millis()-lastNotChargedTime > 10){
      accumulateChargeError += 10;
      lastNotChargedTime = millis();
    }
    if(accumulateChargeError > settings.maxNoPressure){
      strncpy(currentError, "Charge Error", 30);
      returnVal = true;
    }
  } else {
    accumulateChargeError = 0;
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
