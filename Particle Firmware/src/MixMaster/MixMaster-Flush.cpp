#include "MixMaster.h"

// Pump handler
void updatePumps(){
  MixMaster.runPumps();
}

void mixMaster::startFlush(){
  mixerState = FLUSHING;
  FlushingState = FLUSH_INIT;
}



// 3 Charge with air for a settable amount of time (0-5 sec allow millisec settings) set initially to 500 millisec stop when you reach
//   time or settable pressure 0-5 psi set initially to .5 psi which ever comes first - do this every 0-30 sec settable (set initially to 2
//   sec ( runs for entire cycle)

// 4 Pump Motors will run forward for a setable amount of counts (0-6000) initial setting 3000 counts

// 5 Pumps will pause for a setable amount of time (0-5) seconds set for 0 second initially
// 7 Pump Motors will run reverse for a setable amount of counts (0-6000) initial setting 200 counts
// 8 Pumps will pause for a setable amount of time (0-5) seconds set for 0 seconds initially
// 9 Pump Motors will run forward for for a setable amount of counts (0-6000) initial setting 200 counts

// 10 Steps 5-9 will repeat until both liquid presence sensor read empty

// 11 Once complete system will beep 4 times with 1 sec delay between each beep and cycle will stop



void mixMaster::updateFlushing(){  
  static uint32_t timeStartedPause = 0;
  static uint32_t timeStartedCharge = 0;

  // Interval Charge as needed
  intervalCharge();

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
      washCount = 0;
      flushCount = 0;
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
        if(flushCount >= settings.flushCycles){
          FlushingState = FLUSH_FINISH_DELAY;
          timeStartedPause = millis();
        }else{
          FlushingState = FLUSH_SETUP_WASH_REVERSE;
        }
        

      }else if(lastMove == FLUSH_WASH_FORWARD){
        washCount++;

        // if all wash cycles complete, purge, otherwise start a new wash cycle
        if(washCount >= settings.washCycles) FlushingState = FLUSH_SETUP_PURGE;
        else FlushingState = FLUSH_SETUP_WASH_REVERSE;
        
      }else if(lastMove == FLUSH_WASH_REVERSE){
        // Wash reverse is always followed by Wash Forward
        FlushingState = FLUSH_SETUP_WASH_FORWARD;
      }

      break;

    case FLUSH_SETUP_PURGE:
      Serial.printlnf("Start Purge: %d", flushCount);

      ResinPump.setCurrentPosition(0);
      HardenerPump.setCurrentPosition(0);

      if(initialPurge){
        ResinPump.moveTo(settings.initialPurgeCounts);
        HardenerPump.moveTo(settings.initialPurgeCounts);
        FlushingState = FLUSH_AIR_CHARGE;
        timeStartedCharge = millis();
      }else{
        ResinPump.moveTo(settings.purgeCounts);
        HardenerPump.moveTo(settings.purgeCounts);
        FlushingState = FLUSH_RUN_PUMPS;
      }
      
      lastMove = FLUSH_PURGE;
      break;

    case FLUSH_SETUP_WASH_FORWARD:
      Serial.printlnf("Start Wash Forward: %d", washCount);
      
      ResinPump.setCurrentPosition(0);
      HardenerPump.setCurrentPosition(0);
      ResinPump.moveTo(settings.washCounts);
      HardenerPump.moveTo(settings.washCounts);

      lastMove = FLUSH_WASH_FORWARD;

      FlushingState = FLUSH_RUN_PUMPS;

      break;

    case FLUSH_SETUP_WASH_REVERSE:
      Serial.println("Start Wash Reverse");
      
      ResinPump.setCurrentPosition(0);
      HardenerPump.setCurrentPosition(0);
      ResinPump.moveTo(-settings.washCounts);
      HardenerPump.moveTo(-settings.washCounts);
      
      lastMove = FLUSH_WASH_REVERSE;

      FlushingState = FLUSH_RUN_PUMPS;
      timeStartedCharge = millis();

      break;

    case FLUSH_AIR_CHARGE:
      PressureManager.updateTargetPressure(settings.flushChargePressure-500, 0);
      PressureManager.updateTargetPressure(settings.flushChargePressure, 1);
      PressureManager.allowCharging();
      if(millis() - timeStartedCharge > settings.chargeTime){
        Serial.printlnf("Did Charge %d", millis() - timeStartedCharge);
        FlushingState = FLUSH_RUN_PUMPS;
        enableIntervalCharge = true;
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
    
    case FLUSH_FINISH_DELAY:
      if(millis() - timeStartedPause > settings.flushFinishDelay){
        Serial.printlnf("Did Flush Finish Delay: %d", millis() - timeStartedPause);
        FlushingState = FLUSH_SIGNAL_COMPLETE;
      }
      break;

    case FLUSH_SIGNAL_COMPLETE:
      static uint32_t chimeCount = 0;
      static uint32_t chimeTimer = 0;

      // Set Charging pressures back to mixing
      enableIntervalCharge = false;
      PressureManager.updateTargetPressure(settings.offPressure, 0);
      PressureManager.updateTargetPressure(settings.onPressure, 1);

      // Chime 4 times with 1 second between, then reset chime count and exit flushing
      if(chimeCount < 4){
        if(millis() - chimeTimer > 1000+750){
          Serial.printlnf("Chime ON: %d", chimeCount);
          chimeTimer = millis();
          tone(CHIME_PIN, CHIME_FREQUENCY, 750);
          chimeCount++;
        }
      } else {
        chimeCount = 0;
        mixerState = START_IDLE;
      }
      break;

    default:
      FlushingState = FLUSH_INIT;
      break;
  }
}

void mixMaster::intervalCharge(){
  if(enableIntervalCharge){
    if(IntervalChargeState == INTERVAL_CHARGE_AIR_ON){
      PressureManager.allowCharging();
      if(millis() - timeIntervalChargeStarted > settings.chargeTime){
        // Serial.printlnf("Did Interval Charge: %d", millis() - timeIntervalChargeStarted);
        IntervalChargeState = INTERVAL_CHARGE_AIR_PAUSE;
        timeIntervalChargeStarted = millis();
      }
    }else if(IntervalChargeState == INTERVAL_CHARGE_AIR_PAUSE){
      if(millis() - timeIntervalChargeStarted > settings.flushChargeInterval){
        // Serial.printlnf("Did Interval Pause: %d", millis() - timeIntervalChargeStarted);
        IntervalChargeState = INTERVAL_CHARGE_AIR_ON;
        timeIntervalChargeStarted = millis();
      }
    }
  } else {
    timeIntervalChargeStarted = millis();
  }
}