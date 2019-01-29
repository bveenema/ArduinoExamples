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
  static uint32_t timeStartedCharge = 0;
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
        // Reset the wash count
        washCount = 0;

        // increment the flush count if not the inital purge
        if(initialPurge) initialPurge = false;
        else flushCount += 1;

        // if all flush cycles complete, end flushing, otherwise start washing again
        if(flushCount >= settings.flushCycles) mixerState = START_IDLE;
        else FlushingState = FLUSH_SETUP_WASH_REVERSE;

      }else if(lastMove == FLUSH_WASH_FORWARD){
        //increment the wash count
        washCount += 1;

        // if all wash cycles complete, purge, otherwise start a new wash cycle
        if(washCount >= settings.washCycles) FlushingState = FLUSH_SETUP_PURGE;
        else FlushingState = FLUSH_SETUP_WASH_REVERSE;

      }else if(lastMove == FLUSH_WASH_REVERSE){
        // Wash Reverse is always followed by Wash Forward
        FlushingState = FLUSH_SETUP_WASH_FORWARD;

      }

      break;

    case FLUSH_SETUP_PURGE:
      Serial.println("message:Start Purge");

      ResinPump.setCurrentPosition(0);
      HardenerPump.setCurrentPosition(0);

      if(initialPurge){
        ResinPump.moveTo(settings.initialPurgeCounts);
        HardenerPump.moveTo(settings.initialPurgeCounts);
      } else {
        ResinPump.moveTo(settings.purgeCounts);
        HardenerPump.moveTo(settings.purgeCounts);
      }

      lastMove = FLUSH_PURGE;

      FlushingState = FLUSH_AIR_CHARGE;
      timeStartedCharge = millis();

      break;

    case FLUSH_SETUP_WASH_FORWARD:
      Serial.printlnf("message:Start Wash Forward %d", washCount);
      
      ResinPump.setCurrentPosition(0);
      HardenerPump.setCurrentPosition(0);
      ResinPump.moveTo(settings.washCounts);
      HardenerPump.moveTo(settings.washCounts);

      lastMove = FLUSH_WASH_FORWARD;

      FlushingState = FLUSH_RUN_PUMPS;

      break;

    case FLUSH_SETUP_WASH_REVERSE:
      Serial.println("message:Start Wash Reverse");
      
      ResinPump.setCurrentPosition(0);
      HardenerPump.setCurrentPosition(0);
      ResinPump.moveTo(-settings.washCounts);
      HardenerPump.moveTo(-settings.washCounts);
      
      lastMove = FLUSH_WASH_REVERSE;

      FlushingState = FLUSH_AIR_CHARGE;
      timeStartedCharge = millis();

      break;

    case FLUSH_AIR_CHARGE:
      PressureManager.forcePumpOn();
      if(millis() - timeStartedCharge > settings.chargeTime){
        Serial.printlnf("message:Did Charge %d", millis() - timeStartedCharge);
        PressureManager.forcePumpOff();
        FlushingState = FLUSH_RUN_PUMPS;
      }
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