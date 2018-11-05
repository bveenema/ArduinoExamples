#ifndef PressureManager_H
#define PressureManager_H

#include "Particle.h"
#include "config.h"
#include "globals.h"

class pressureManager{
public:
  // Constructor: takes targetPressure (milli-inH20) agrument - defaults to 10 inH20
  pressureManager(){}

  // Initialize air pump and pressure sensor
  void init(uint32_t onPressure = defaultSettings.onPressure,
            uint32_t offPressure = defaultSettings.offPressure);

  // Charge the Pails to the target pressure if isCharging is true
  void update();

  // returns true when the air pump needs to turn on
  bool requestCharging();

  // Air pump is allowed to turn on if allowCharging has been called in last 10ms
  void allowCharging();

  // Force Pressure Manager into "At Pressure" state
  void forceAtPressure();

  // Force Pressure Manager into "Under Pressure" state
  void forceUnderPressure();

  // Returns true when at or above offPressure
  bool isCharged();

  // Function to set whether pails should be charged (true) or not (false)
  void setChargingState(bool charge);

  int32_t updateTargetPressure(int32_t pressure, bool type = 0);

  int32_t getPressure();
private:
  // True if Pails are charged
  bool charged = false;

  bool _atPressure = false;

  bool _requestCharging = false;
  uint32_t _lastAllowChargingCall = 0;
  bool _allowCharging = false;
  bool _currentChargingState = false;

  // True if pails should be charged
  bool isCharging = false;

  // Pressure to Maintain in the Pails
  int32_t _onPressure = 0; // milli-inH20
  int32_t _offPressure = 0;

  // Timeout
  uint32_t chargingTimer = 0;
  uint32_t chargingTimeout = 15000;

  // Pressure Container
  struct{
    int32_t current = 0;
    uint16_t samples[100] = {0};
    uint8_t index = 0;
    uint8_t length = 100;
    bool isValid = false;
    uint32_t lastRead = 0;
    uint32_t accumulateUnderPressure = 0;
  } _pressure;

  // Reads the pressure sensor and returns the value in milli-inH2O
  uint16_t readPressure();
};

extern pressureManager PressureManager;

#endif
