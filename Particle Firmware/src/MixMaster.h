#ifndef MixMaster_h
#define MixMaster_h

#include "Particle.h"
#include "AccelStepper.h"
#include "globals.h"

typedef enum{
  StartIdle, // 0
  Idle, // 1
  Charging, // 2
  Mixing, // 3
  StartAutoReverse, // 4
  AutoReversing, // 5
  Flushing // 6
} MixerState;

typedef enum{
  Resin,
  Hardener
} MixerChannel;

class mixMaster{
public:
  mixMaster();

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

  // Puts Mix Master in to Flushing state
  //  * Mix Master can only be taken in to Flushing state through this function
  //  * The "Flushing" mixerState calls updateFlushing() and checks for _changestate
  //  * if _changeState is true while "Flushing" Mixer State becomes "StartIdle"
  //  * startFlushing records the time "Flushing" started, "Flushing" state checks
  //      if "Flushing" has been running for longer than FLUSH_CYCLE_DURATION
  void startFlush();

private:
  // Pump objects
  AccelStepper ResinPump;
  AccelStepper HardenerPump;

  // Pump Control functions
  // Calculate the resinPumpSpeed and hardenerPumpSpeed, enable the pumps, return the time to pump
  uint32_t prepForMixing(uint32_t volume, uint32_t flowRate);
  void idlePumps();
  void runPumps();
  bool runPumpsWithErrorCheck();

  // Current State of Mix Master
  MixerState mixerState = StartIdle;

  // Pump configuration
  const unsigned int ultimateMaxSpeed = 15000;
  const unsigned int autoReverseSpeed = 4000;

  // Pump Info
  uint32_t resinPumpSpeed = 0; //steps/s
  uint32_t hardenerPumpSpeed = 0; //steps/s

  // Flushing States
  enum FlushingState{
    InitFlush,
    PulseOn,
    IdleFlushing
  } FlushingState;

  // Flushing State Machine Handler
  void updateFlushing();

  // Flushing Timer
  uint32_t timeStartedFlushing;
};


#endif
