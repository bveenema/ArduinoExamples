#ifndef CHIME_H
#define CHIME_H

#include "Particle.h"
#include "config.h"
#include "LiquidSensor.h"
#include "TemperatureMonitor.h"
#include "MixMaster.h"

class chime{
public:
  chime();

  // Initiates Pins
  void init();

  // Check for errors and signal appropriate chime
  void update();

  // Silence the Chime until the error resets
  void silence();

private:
  bool _silenced = false; // blocks the chime when true
  bool _chiming = false; // true when a multi-cycle chime is needed, prevents _chimeState from changing

  enum ChimeStates {
    OFF,
    SLOW,
    MEDIUM,
    FAST,
    SIGNAL_END_FLUSHING,
  } _chimeState;
  uint32_t _lastChime = 0;
  uint32_t _fastChimeCount = 0; // Fast chimes 3 times, then pauses
};

extern chime Chime;

#endif
