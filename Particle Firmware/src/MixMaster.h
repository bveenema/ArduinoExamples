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

  // setup pins and pump configurations
  void init();

  // Updates the mixer state handler (ie. the main "brain" of the mixer)
  //  if _changeState is passed true, update will process if in Idle or Mixing states
  //  and return it as false when it has processed it.
  bool update(bool _changeState);

  // Get the current state of the mixer
  MixerState getState();

  // returns the currently calculated pump speeds
  uint32_t getPumpSpeed(MixerChannel channel);

  // Puts Mix Master in to Cleaning state
  //  * Mix Master can only be taken in to Cleaning state through this function
  //  * The "Cleaning" mixerState calls updateCleaning() and checks for _changestate
  //  * if _changeState is true while "Cleaning" Mixer State becomes "Idle"
  //  * startCleaning records the time "Cleaning" started, "Cleaning" state checks
  //      if "Cleaning" has been running for longer than cleaningTime
  void startCleaning();

private:
  // Pump objects
  AccelStepper ResinPump;
  AccelStepper HardenerPump;

  // Pump Control functions
  // Calculate the resinPumpSpeed and hardenerPumpSpeed, enable the pumps, return the time to pump
  uint32_t prepForMixing(uint32_t volume, uint32_t flowRate);
  void idlePumps();
  void runPumps();

  // Current State of Mix Master
  MixerState mixerState = Idle;

  // Pump configuration
  const unsigned int ultimateMaxSpeed = 15000;
  const unsigned int autoReverseSpeed = 4000;

  // Pump Info
  uint32_t resinPumpSpeed = 0; //steps/s
  uint32_t hardenerPumpSpeed = 0; //steps/s

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
