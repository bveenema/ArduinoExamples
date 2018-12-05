#include "MixMaster.h"

/*
selector set to zero
Both pump will run exact same flow rate
Pump Motors will run forward for a setable amount of counts (0-2000) initial setting 200 counts
Pumps will pause for a setable amount of time (0-5) seconds set for 1 second initially
Pump Motors will run reverse for a setable amount of counts (0-2000) initial setting 180 counts
Pumps will pause for a settable amount of time (0-5) seconds set for 1 second initially
This will continue for a setable amount of cleaning time (0-30) min set initially for 30 min
RPM will be a setable value (0-400) set initially for 200
*/



// Pump handler
void updatePumps(){
  MixMaster.runPumps();
}

void mixMaster::startFlush(){
  mixerState = FLUSHING;
  FlushingState = INITFLUSH;
  timeStartedFlushing = millis();
}

void mixMaster::updateFlushing(){
  static uint32_t flushTime;
  static uint32_t timeStateStarted;


  /*
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
  */

 // enum FlushingState {
//   FLUSH_INIT,
//   FLUSH_FORWARD,
//   FLUSH_PAUSE_1,
//   FLUSH_REVERSE,
//   FLUSH_PAUSE_2
// } FlushingState;

  static uint32_t timeStartedPause = 0;
  switch(FlushingState){
    case FLUSH_INIT:
      // We don't know current pump state, so assume it's running and stop it
      this->idlePumps();

      // prep for mixing, Ratio Resin to Hardener = 100:100, first param, volume per pulse is not needed
      prepForMixing(1000, FLUSH_FLOW_RATE, 100, 100);

      flushTime = constrain(settings.flushTime, FLUSH_MIN_TIME-1, FLUSH_MAX_TIME+1);

      timeStateStarted = millis();
      IOExp.digitalWrite(RESIN_ENABLE_IOEXP_PIN, LOW); // Enable Resin Pump
      IOExp.digitalWrite(HARDENER_ENABLE_IOEXP_PIN, LOW); // Enable Hardener Pump

      uint32_t flushSpeed = RPMtoStepsPerSecond(settings.flushRPM);
      ResinPump.setMaxSpeed(flushSpeed);
      HardenerPump.setMaxSpeed(flushSpeed);
      ResinPump.setCurrentPosition(0);
      HardenerPump.setCurrentPosition(0);
      ResinPump.moveTo(settings.flushForwardSteps);
      HardenerPump.moveTo(settings.flushForwardSteps);

      FlushingState = FLUSH_FORWARD;

      break;

    case FLUSH_FORWARD:
      if(this->checkPumpErrors()) mixerState = START_IDLE;
      ResinPump.run();
      HardenerPump.run();
      if(!ResinPump.isRunning() && !HardenerPump.isRunning()){
        FlushingState = FLUSH_PAUSE_1;
        timeStartedPause = millis();
      }

      break;
    
    case FLUSH_PAUSE_1:
      if(millis() - timeStartedPause > settings.flushForwardPause){
        FlushingState = FLUSH_REVERSE;
        ResinPump.setMaxSpeed(flushSpeed);
        HardenerPump.setMaxSpeed(flushSpeed);
        ResinPump.setCurrentPosition(0);
        HardenerPump.setCurrentPosition(0);
        ResinPump.moveTo(-settings.flushReverseSteps);
        HardenerPump.moveTo(-settings.flushReverseSteps);
      }
      break;

    case FLUSH_REVERSE:
      if(this->checkPumpErrors()) mixerState = START_IDLE;
      ResinPump.run();
      HardenerPump.run();
      if(!ResinPump.isRunning() && !HardenerPump.isRunning()){
        FlushingState = FLUSH_PAUSE_2;
        timeStartedPause = millis();
      }

      break;

    default:
      FlushingState = FLUSH_INIT;
      break;
  }
}
