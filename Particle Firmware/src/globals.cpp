#include "globals.h"

// Prime Info
bool isPrimed = false;

// Wifi Status
 uint32_t wifiStatus = 0;

// Error Status
char currentError[30] = "none";

int selector = 0;

// State Machine
bool changeState = false;

bool FLAG_wasError = 0;
bool FLAG_justReset = 0;

// Settings
prom_settings settings;


// Debugging Variables
uint32_t debug_incrementer = 0;
