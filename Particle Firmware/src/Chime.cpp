#include "Chime.h"

chime Chime;

chime::chime(){}

void chime::init(){
  pinMode(CHIME_PIN, OUTPUT);
  pinResetFast(CHIME_PIN);
  _chimeState = OFF;
  _silenced = false;
}

void chime::update(){
  // Check for errors
  if(!_chiming){
      if((!ResinLiquidSensor.hasLiquid() || !HardenerLiquidSensor.hasLiquid()) && MixMaster.mixerState != FLUSHING)  _chimeState = FAST;
    else if(!ResinTemp.isInRange() || !HardenerTemp.isInRange())  _chimeState = SLOW;
    else _chimeState = OFF;
  }

  // Create chime if not silenced
  if(!_silenced && _chimeState != OFF){
    int chimeRate = 0;
         if(_chimeState == SLOW) chimeRate = CHIME_SLOW_RATE;
    else if(_chimeState == MEDIUM) chimeRate = CHIME_MEDIUM_RATE;
    else if(_chimeState == FAST){
      if(_fastChimeCount < 2){
        chimeRate = CHIME_PULSE_TIME;
      }else{
        chimeRate = CHIME_FAST_RATE;
      }
    }

    if(millis() - _lastChime > chimeRate){
      tone(CHIME_PIN, CHIME_FREQUENCY, CHIME_PULSE_TIME);
      _lastChime = millis();
      if(_fastChimeCount < 2) _fastChimeCount++;
      else _fastChimeCount = 0;
    }
  }

  // reset _silenced if no error
  if(_chimeState == OFF) _silenced = false;
}

void chime::silence(){
  _silenced = true;
}
