#ifndef MixMaster_h
#define MixMaster_h

#include "AccelStepper.h"
#include "globals.h"

typedef enum{
  Idle,
  MixingCalculations,
  Mixing,
  StartAutoReverse,
  AutoReversing
} MixerState;

typedef enum{
  Resin,
  Hardener
} MixerChannel;

class MixMaster{
public:
  MixMaster();
  void init();
  bool update(bool _changeState);

  MixerState getState();

  uint32_t getMotorSpeed(MixerChannel channel);

private:
  // Create Motor objects
  AccelStepper motorResin;
  AccelStepper motorHardener;

  // Current State of Mix Master
  MixerState mixerState = Idle;

  // Motor configuration
  const unsigned int ultimateMaxSpeed = 15000;
  const unsigned int autoReverseSpeed = 4000;

  // Motor Info
  uint32_t motorSpeedResin = 0; //steps/s
  uint32_t motorSpeedHardener = 0; //steps/s
};


#endif
