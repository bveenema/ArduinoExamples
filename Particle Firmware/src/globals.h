#ifndef GLOBALS_H
#define GLOBALS_H


#include "Particle.h"
#include "config.h"

// Wifi Status
extern uint32_t wifiStatus;

// Error Status
extern char currentError[30];

extern int selector;

// State Machine
extern bool changeState;

// Motor Info
extern uint32_t motorSpeedResin; //steps/s
extern uint32_t motorSpeedHardener; //steps/s
extern const unsigned int ultimateMaxSpeed;
extern const unsigned int autoReverseSpeed;

extern bool FLAG_wasError;
extern bool FLAG_justReset;

// Settings
extern prom_settings settings;

#endif
