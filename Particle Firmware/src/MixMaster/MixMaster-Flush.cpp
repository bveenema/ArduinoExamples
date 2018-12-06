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
  FlushingState = FLUSH_INIT;
  timeStartedFlushing = millis();
}

void mixMaster::updateFlushing(){
  if(millis() - timeStartedFlushing > settings.flushTime*60*1000){
    mixerState = START_IDLE;
  }
  
  static uint32_t timeStartedPause = 0;
  switch(FlushingState){
    case FLUSH_INIT:{
      // We don't know current pump state, so assume it's running and stop it
      this->idlePumps();

      // prep for mixing, Ratio Resin to Hardener = 100:100, first param, volume per pulse is not needed
      prepForMixing(1000, FLUSH_FLOW_RATE, 100, 100);

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
    }

    case FLUSH_FORWARD:
      ResinPump.run();
      HardenerPump.run();
      if(!ResinPump.isRunning() && !HardenerPump.isRunning()){
        FlushingState = FLUSH_FORWARD_PAUSE;
        timeStartedPause = millis();
      }

      break;
    
    case FLUSH_FORWARD_PAUSE:
      if(millis() - timeStartedPause > settings.flushForwardPause){
        FlushingState = FLUSH_REVERSE;
        ResinPump.setCurrentPosition(0);
        HardenerPump.setCurrentPosition(0);
        ResinPump.moveTo(-settings.flushReverseSteps);
        HardenerPump.moveTo(-settings.flushReverseSteps);
      }
      break;

    case FLUSH_REVERSE:
      ResinPump.run();
      HardenerPump.run();
      if(!ResinPump.isRunning() && !HardenerPump.isRunning()){
        FlushingState = FLUSH_REVERSE_PAUSE;
        timeStartedPause = millis();
      }
      break;

    case FLUSH_REVERSE_PAUSE:
      if(millis() - timeStartedPause > settings.flushReversePause){
        FlushingState = FLUSH_FORWARD;
        ResinPump.setCurrentPosition(0);
        HardenerPump.setCurrentPosition(0);
        ResinPump.moveTo(settings.flushForwardSteps);
        HardenerPump.moveTo(settings.flushForwardSteps);
      }
      break;

    default:
      FlushingState = FLUSH_INIT;
      break;
  }
}
