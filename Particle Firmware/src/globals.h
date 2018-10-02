#ifndef GLOBALS_H
#define GLOBALS_H


#include "Particle.h"
#include "config.h"
#include "PressureManager.h"

// Prime Info
extern bool isPrimed;

// Wifi Status
extern uint32_t wifiStatus;

// Error Status
extern char currentError[30];

extern int selector;

// Pressure Manager
extern pressureManager PressureManager;

// State Machine
extern bool changeState;

// Pump Info
extern const unsigned int ultimateMaxSpeed;
extern const unsigned int autoReverseSpeed;

// Settings
extern prom_settings settings;

extern bool FLAG_wasError;
extern bool FLAG_justReset;

#endif
