#include "MixMaster.h"

// Pump handler
void updatePumps(){
  MixMaster.runPumps();
}

void mixMaster::startFlush(){
  mixerState = FLUSHING;
  FlushingState = FLUSH_INIT;
  washCount = 0;
  flushCount = 0;
}

void mixMaster::updateFlushing(){
  // Check flushCount for done-ness
  if(flushCount >= settings.flushCycles) mixerState = START_IDLE;
  
  static uint32_t timeStartedPause = 0;
  switch(FlushingState){
    case FLUSH_INIT:{
      // We don't know current pump state, so assume it's running and stop it
      this->idlePumps();

      // prep for mixing, Ratio Resin to Hardener = 100:100, first param, volume per pulse is not needed
      prepForMixing(1000, FLUSH_FLOW_RATE, 100, 100);

      enablePumps();

      // Set the flush speed for each channel
      ResinPump.setMaxSpeed(getSpeedFromRPM(settings.flushRPM, settings.stepsPerMlResin, settings.stepsPerMlHardener, 1, 1));
      HardenerPump.setMaxSpeed(getSpeedFromRPM(settings.flushRPM, settings.stepsPerMlResin, settings.stepsPerMlHardener, 1, 1));

      FlushingState = FLUSH_SETUP_PURGE;
      initialPurge = true;
      break;
    }

    case FLUSH_CHECK:
      if(lastMove == FLUSH_PURGE){
        // increment the flush count if not the inital purge
        if(initialPurge) initialPurge = false;
        else flushCount += 1;

        // if all flush cycles complete, end flushing, otherwise start washing again
        if(flushCount >= settings.flushCycles) mixerState = START_IDLE;
        else FlushingState = FLUSH_SETUP_WASH_FORWARD;

        // reset Wash Count
        washCount = 0;

      }else if(lastMove == FLUSH_WASH_FORWARD){
        // Wash forward is always followed by Wash Reverse
        FlushingState = FLUSH_SETUP_WASH_REVERSE;

      }else if(lastMove == FLUSH_WASH_REVERSE){
        //increment the wash count
        washCount += 1;

        // if all wash cycles complete, purge, otherwise start a new wash cycle
        if(washCount >= settings.washCycles) FlushingState = FLUSH_SETUP_PURGE;
        else FlushingState = FLUSH_SETUP_WASH_FORWARD;
      }

      break;

    case FLUSH_SETUP_PURGE:
      Serial.println("message:Start Purge");

      ResinPump.setCurrentPosition(0);
      HardenerPump.setCurrentPosition(0);
      ResinPump.moveTo(settings.purgeCounts);
      HardenerPump.moveTo(settings.purgeCounts);

      FlushingState = FLUSH_RUN_PUMPS;
      lastMove = FLUSH_PURGE;

      break;

    case FLUSH_SETUP_WASH_FORWARD:
      Serial.println("message:Start Wash Forward");
      
      ResinPump.setCurrentPosition(0);
      HardenerPump.setCurrentPosition(0);
      ResinPump.moveTo(settings.washCounts);
      HardenerPump.moveTo(settings.washCounts);

      FlushingState = FLUSH_RUN_PUMPS;
      lastMove = FLUSH_WASH_FORWARD;

      break;

    case FLUSH_SETUP_WASH_REVERSE:
      Serial.println("message:Start Wash Reverse");
      
      ResinPump.setCurrentPosition(0);
      HardenerPump.setCurrentPosition(0);
      ResinPump.moveTo(-settings.washCounts);
      HardenerPump.moveTo(-settings.washCounts);

      FlushingState = FLUSH_RUN_PUMPS;
      lastMove = FLUSH_WASH_REVERSE;

      break;

    case FLUSH_RUN_PUMPS:
      ResinPump.run();
      HardenerPump.run();
      if(!ResinPump.isRunning() && !HardenerPump.isRunning()){
        FlushingState = FLUSH_PAUSE;
        timeStartedPause = millis();
      }

      break;
    
    case FLUSH_PAUSE:
      if(millis() - timeStartedPause > settings.flushPause){
        FlushingState = FLUSH_CHECK;
      }
      break;

    default:
      FlushingState = FLUSH_INIT;
      break;
  }
}
