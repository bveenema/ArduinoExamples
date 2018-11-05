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
  static uint32_t flushPulseTime;
  static uint32_t flushIdleTime;
  static uint32_t timeStateStarted;

  switch(FlushingState){
    case INITFLUSH:
      // We don't know current pump state, so assume it's running and stop it
      this->idlePumps();

      // Calculate Flush Pulse and Idle times, prep for mixing, Ratio Resin to Hardener = 100:100
      flushPulseTime = prepForMixing(FLUSH_VOLUME_PER_PULSE, FLUSH_FLOW_RATE, 100, 100);
      if(flushPulseTime < FLUSH_PULSE_INTERVAL){
        flushIdleTime = FLUSH_PULSE_INTERVAL - flushPulseTime;
      } else {
        flushIdleTime = 0;
      }

      timeStateStarted = millis();
      IOExp.digitalWrite(RESIN_ENABLE_IOEXP_PIN, LOW); // Enable Resin Pump
      IOExp.digitalWrite(HARDENER_ENABLE_IOEXP_PIN, LOW); // Enable Hardener Pump
      pumpUpdater.begin(updatePumps, 10, uSec);

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
      FlushingState = INITFLUSH;
      break;
  }
}
