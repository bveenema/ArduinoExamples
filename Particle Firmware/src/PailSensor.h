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

  uint32_t getCurrentReading();

  bool getState();

private:
  uint32_t detectionThreshold = DEFAULT_PAIL_DETECTION_THRESHOLD;
  uint32_t currentReading = 0;
  struct {
    uint16_t values[100] = {0};
    uint32_t index = 0;
    uint32_t length = 100;
    bool areValid = false;
  } readings;

  bool state = false;

  uint16_t readSensor();

};

extern pailSensor PailSensor;

#endif
