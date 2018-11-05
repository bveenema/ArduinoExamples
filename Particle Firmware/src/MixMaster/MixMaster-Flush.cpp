#include "MixMaster.h"

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

  switch(FlushingState){
    case INITFLUSH:
      // We don't know current pump state, so assume it's running and stop it
      this->idlePumps();

      // prep for mixing, Ratio Resin to Hardener = 100:100, first param, volume per pulse is not needed
      prepForMixing(1000, FLUSH_FLOW_RATE, 100, 100);

      flushTime = constrain(settings.flushTime, FLUSH_MIN_TIME-1, FLUSH_MAX_TIME+1);

      timeStateStarted = millis();
      IOExp.digitalWrite(RESIN_ENABLE_IOEXP_PIN, LOW); // Enable Resin Pump
      IOExp.digitalWrite(HARDENER_ENABLE_IOEXP_PIN, LOW); // Enable Hardener Pump
      pumpUpdater.begin(updatePumps, 10, uSec);

      FlushingState = FLUSH;

      break;

    case FLUSH:
      if(this->checkPumpErrors()) mixerState = START_IDLE;
      if(millis()-timeStateStarted > flushTime){
        mixerState = START_IDLE;
        pumpUpdater.end();
      }

      break;

    default:
      FlushingState = INITFLUSH;
      break;
  }
}
