#include "MixMaster.h"

// Pump handler
void updatePumpsMainState(){
  debug_incrementer++;
  MixMaster.runPumps();
}

bool mixMaster::update(bool _changeState){
  static uint32_t timeToMix = 0;
  static uint32_t timeStartedCharging = 0;
  static uint32_t timeStartedIdling = 0;
  static uint32_t accumulatedMixingTime = 0;
  static bool keepOpen = false;
  static bool prime = false;
  static bool runZeroDrip = false;

  if(mixerState == START_IDLE){ // 0
    if(numConsecutivePrimes >= settings.minPrimes) isPrimed = true;
    timeStartedIdling = millis();
    mixerState = IDLE;
  }else if(mixerState == IDLE){ // 1
    if(!this->ZeroDrip(runZeroDrip)) this->idlePumps();
    if(!runZeroDrip && PailSensor.getState() && selector != 0) runZeroDrip = true;

    if(_changeState || (millis()- timeStartedIdling > TIME_BETWEEN_KEEP_OPEN_CYCLES)) {
      // If no Pail in position or liquid, prevent prime, keep open or mixing
      if(!PailSensor.getState() || !ResinLiquidSensor.hasLiquid() || !HardenerLiquidSensor.hasLiquid()){
        if(!_changeState && !PailSensor.getState()) runZeroDrip = false;
        mixerState = START_IDLE;
        _changeState = false;
        return _changeState;
      }
      if(_changeState){
        (!isPrimed) ? prime=true : prime=false;
        keepOpen = false;
        _changeState = false;
      } else { // keep open
        // prevent keepOpen if not primed or selector is in flush position
        if(!isPrimed || selector == 0){
          mixerState = START_IDLE;
          return _changeState;
        }
        keepOpen = true;
        prime = false;
      }
      mixerState = MIXING_CALCULATIONS;
    }
  }else if(mixerState == MIXING_CALCULATIONS){ // 2
    if(prime) timeToMix = this->prepForMixing(settings.primeVolume, settings.flowRate[selector], settings.ratioResin[selector], settings.ratioHardener[selector]);
    else if(keepOpen) timeToMix = this->prepForMixing(settings.keepOpenVolume, settings.flowRate[selector], settings.ratioResin[selector], settings.ratioHardener[selector]);
    else timeToMix = this->prepForMixing(settings.volume[selector], settings.flowRate[selector], settings.ratioResin[selector], settings.ratioHardener[selector]);
    timeStartedCharging = millis();
    accumulatedMixingTime = 0;
    mixerState = MIXING;
    MixingState = CHARGING;
    PressureManager.forceUnderPressure();
  }else if(mixerState == MIXING){ // 3
    // 1) Turn on air charge and run for a settable time (0-60) or until you reach upper pressure set point programmable 0-2 psi
    // 2) Turn off Air
    // 3) Pause for time settable in program 1-3 seconds
    // 4) Start cycle (mix) and run until you reach lower pressure set point 0-2 psi and repeat 1-3 until mix volume is complete
    static uint32_t timeEndedCharging = 0;
    static uint32_t timeStartedMixing = 0;
    uint32_t now = millis();
    if(MixingState == CHARGING){
      PressureManager.allowCharging();
      if(PressureManager.isCharged() || (now - timeStartedCharging > settings.chargeTimeout)) {
        PressureManager.forceAtPressure(); // ensure pressure is in "at pressure" state
        timeEndedCharging = now;
        MixingState = DELAY;
      }
    }else if(MixingState == DELAY){
      if(now - timeEndedCharging > settings.chargeDelay){
        timeStartedMixing = now;
        MixingState = MIX;
        pumpUpdater.begin(updatePumpsMainState, 10, uSec); // resume pumping
      }
    }else if(MixingState == MIX){
      if((now - timeStartedMixing + accumulatedMixingTime) > timeToMix){
        accumulatedMixingTime += now - timeStartedMixing;
      }
      if(PressureManager.requestCharging()){
        accumulatedMixingTime += now - timeStartedMixing;
        timeStartedCharging = now;
        pumpUpdater.end();
        MixingState = CHARGING;
      }
    }

    if(_changeState || !PailSensor.getState() || !ResinLiquidSensor.hasLiquid() || !HardenerLiquidSensor.hasLiquid()){
      pumpUpdater.end(); // kill pump interrupt handler
      // don't reset _changeState when keep open so button won't be "ignored" while keep open
      if(!keepOpen) _changeState = false;
      mixerState = START_IDLE;
    }

    if(accumulatedMixingTime > timeToMix){
      pumpUpdater.end(); // kill pump interrupt handler
      if(settings.autoReverseSteps > 0) mixerState = START_AUTO_REVERSE;
      else mixerState = START_IDLE;
      // if the cycle was a prime and it got here, increment numConsecutivePrimes
      if(prime){
        numConsecutivePrimes += 1;
      }
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
      mixerState = START_IDLE;
    }
  }else if(mixerState == FLUSHING){ // 6
    // kill Flusing if no pail or liquid
    if(!PailSensor.getState() || !ResinLiquidSensor.hasLiquid() || !HardenerLiquidSensor.hasLiquid()){
      mixerState = START_IDLE;
      _changeState = false;
      return _changeState;
    }
    this->updateFlushing();
    if(_changeState == true){
      mixerState = START_IDLE;
      _changeState = false;
    }
  }

  return _changeState;
}
