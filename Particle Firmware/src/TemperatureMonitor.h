#ifndef TEMPERATURE_MONITOR_H
#define TEMPERATURE_MONITOR_H

#include "Particle.h"
#include "config.h"
#include "thermistor-beta-vdw.h"

class TemperatureMonitor{
public:
  TemperatureMonitor();

  // Initiates Pins
  void init(pin_t pin);

  // Check for errors and signal appropriate TemperatureMonitor
  void update();

  // returns the current temperature
  uint32_t getTemp();

  // returns true if the temperature is within min and max
  bool isInRange();

  // sets the min and max temperature range
  void setRange(int32_t min, int32_t max);

private:
  pin_t _pin = 0;
  ThermistorBetaVDW thermistor;

  uint32_t _lastTemperatureCheck = 0;
  int32_t _currentTemp = 0;
  int32_t _maxTemp = 0;
  int32_t _minTemp = 0;
};

extern TemperatureMonitor ResinTemp;
extern TemperatureMonitor HardenerTemp;

#endif
