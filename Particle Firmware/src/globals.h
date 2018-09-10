#ifndef GLOBALS_H
#define GLOBALS_H


#include "Particle.h"
#include "config.h"

// Wifi Status
extern uint32_t wifiStatus;

// Serial Communication
extern bool FLAG_messageReceived;
extern bool FLAG_isWrite;
extern bool FLAG_isSelectorSetting;
const size_t messageBufferSize = 128;
extern char messageBuffer[messageBufferSize];
extern char variableNameBuffer[32];
extern char valueBuffer[32];
extern uint32_t selectorBuffer;
extern uint8_t messageIndex;

// Error Status
extern char currentError[30];


extern int selector;

// State Machine
extern bool changeState;
extern uint8_t STATE_mixer; //0: Not Moving 1: Mixing 2: Start AutoReverse 3: AutoReversing

// Motor Info
extern uint32_t motorSpeedA; //steps/s
extern uint32_t motorSpeedB; //steps/s
extern const unsigned int ultimateMaxSpeed;
extern const unsigned int autoReverseSpeed;

extern bool FLAG_wasError;
extern bool FLAG_justReset;

// Settings
extern prom_settings settings;

#endif
