#ifndef LIQUID_SENSOR_H
#define LIQUID_SENSOR_H

#include "Particle.h"
#include "config.h"
#include "globals.h"

class liquidSensor{
public:
  liquidSensor();

  // Initiates Pins
  void init(uint8_t pin);

  // Checks Liquid Sensors for presesence
  void update();

  // Returns current state of liquid presence
  bool hasLiquid();

private:
  bool _hasLiquid = false;
  uint8_t _pin = 0;
};

extern liquidSensor ResinLiquidSensor;
extern liquidSensor HardenerLiquidSensor;

#endif
