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
      break;
    }

    case FLUSH_CHECK:

      if(!ResinLiquidSensor.hasLiquid() && !HardenerLiquidSensor.hasLiquid()){
        timeStartedPause = millis();
        FlushingState = FLUSH_SIGNAL_COMPLETE;
      }

      if(lastMove == FLUSH_PURGE){
        // After Purge is complete, enter wash cycles
        FlushingState = FLUSH_SETUP_WASH_REVERSE;

      }else if(lastMove == FLUSH_WASH_FORWARD){
        // Wash forward is always followed by Wash Reverser
        FlushingState = FLUSH_SETUP_WASH_REVERSE;
        
      }else if(lastMove == FLUSH_WASH_REVERSE){
        // Wash reverse is always followed by Wash Forward
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

    case FLUSH_SIGNAL_COMPLETE:
      static uint32_t chimeCount = 0;
      static uint32_t chimeTimer = 0;

      // Chime 4 times with 1 second between, then reset chime count and exit flushing
      if(chimeCount < 4){
        if(millis() - chimeTimer > 1000){
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