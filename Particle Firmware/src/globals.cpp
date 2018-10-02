#include "globals.h"

// Prime Info
bool isPrimed = false;

// Wifi Status
 uint32_t wifiStatus = 0;

// Error Status
char currentError[30] = "none";

int selector = 0;

// Pressure Manager
pressureManager PressureManager(10000);

// State Machine
bool changeState = false;

bool FLAG_wasError = 0;
bool FLAG_justReset = 0;

// Settings
prom_settings settings;
