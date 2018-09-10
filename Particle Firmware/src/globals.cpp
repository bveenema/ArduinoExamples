#include "globals.h"

// Wifi Status
 uint32_t wifiStatus = 0;

// Serial Communication
bool FLAG_messageReceived = false;
bool FLAG_isWrite = false;
bool FLAG_isSelectorSetting = false;
char messageBuffer[messageBufferSize];
char variableNameBuffer[32];
char valueBuffer[32];
uint32_t selectorBuffer = 0;
uint8_t messageIndex = 0;

// Error Status
char currentError[30] = "none";


int selector = 0;

// State Machine
bool changeState = false;
uint8_t STATE_mixer = 0; //0: Not Moving 1: Mixing 2: Start AutoReverse 3: AutoReversing

// Motor Info
uint32_t motorSpeedA = 0; //steps/s
uint32_t motorSpeedB = 0; //steps/s
const unsigned int ultimateMaxSpeed = 15000;
const unsigned int autoReverseSpeed = 4000;

bool FLAG_wasError = 0;
bool FLAG_justReset = 0;

// Settings
prom_settings settings;
