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

class MixMaster{
public:
  MixMaster();
  void init();
  bool update(bool _changeState);

  MixerState getState();

private:
  // Create Motor objects
  AccelStepper motorResin;
  AccelStepper motorHardener;

  MixerState mixerState = Idle;
};


#endif
