#ifndef MixMaster_h
#define MixMaster_h

#include "AccelStepper.h"
#include "globals.h"

typedef enum{
  Idle,
  MixingCalculations,
  Mixing,
  StartAutoReverse,
  AutoReversing,
  Cleaning
} MixerState;

typedef enum{
  Resin,
  Hardener
} MixerChannel;

class MixMaster{
public:
  MixMaster();

  // setup pins and motor configurations
  void init();

  // Updates the mixer state handler (ie. the main "brain" of the mixer)
  //  if _changeState is passed true, update will process if in Idle or Mixing states
  //  and return it as false when it has processed it.
  bool update(bool _changeState);

  // Get the current state of the mixer
  MixerState getState();

  // returns the currently calculated motor speeds
  uint32_t getMotorSpeed(MixerChannel channel);

  // Puts Mix Master in to Cleaning state
  //  * Mix Master can only be taken in to Cleaning state through this function
  //  * The "Cleaning" mixerState calls updateCleaning() and checks for _changestate
  //  * if _changeState is true while "Cleaning" Mixer State becomes "Idle"
  //  * startCleaning records the time "Cleaning" started, "Cleaning" state checks
  //      if "Cleaning" has been running for longer than cleaningTime
  void startCleaning();

private:
  // Motor objects
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

  // Cleaning States
  enum CleaningState{
    InitCleaning,
    PulseOn,
    IdleCleaning
  } CleaningState;

  // Cleaning State Machine Handler
  void updateCleaning();

  // Cleaning Timer
  uint32_t timeStartedCleaning;
  const uint32_t cleaningTime = 600000 // Total time to run cleaning cycle (10 minutes)
};


#endif
