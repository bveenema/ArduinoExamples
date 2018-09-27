#ifndef PAIL_SENSOR_H
#define PAIL_SENSOR_H

#include "Particle.h"
#include "config.h"

class pailSensor{
public:
  pailSensor();

  void init();

  void update();

  void setDetectionThreshold(uint32_t newThreshold);

  bool getState();

private:
  uint32_t detectionThreshold = DEFAULT_PAIL_DETECTION_THRESHOLD;
  bool state = false;

  uint32_t readSensor();

};

static pailSensor PailSensor;

#endif
