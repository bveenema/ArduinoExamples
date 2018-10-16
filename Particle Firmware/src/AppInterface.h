#ifndef AppInterface_h
#define AppInterface_h


#include "Particle.h"
#include "config.h"
#include "MixMaster.h"

// Link to external variables
#include "globals.h"
#include "PailSensor.h"
#include "PressureManager.h"
extern mixMaster MixMaster;

// Command Functions
//      ** must take no arguments and return 'void'
//      ** variables of type other than uint32_t and Read only variables should
//         be returned via Command Function
inline void printCurrentSelector(){ Serial.print(selector); }
inline void printCurrentError(){ Serial.print(currentError); }
inline void printFirmwareID(){ Serial.print(THIS_PRODUCT_ID); }
inline void printMixerState(){ Serial.print(MixMaster.getState()); }
inline void printResinPumpSpeed(){ Serial.print(MixMaster.getPumpSpeed(Resin)); }
inline void printHardenerPumpSpeed(){ Serial.print(MixMaster.getPumpSpeed(Hardener)); }
inline void printNumSelectors(){ Serial.print(NUM_SELECTORS); }
inline void printProductVersion(){ Serial.print(THIS_PRODUCT_VERSION); }
inline void printCloudStatus(){
  if(Particle.connected()){
    Serial.print("Connected");
  }else {
    Serial.print("Not Available");
  }
}
inline void togglePump(){ changeState = true; Serial.print("N/A"); }
inline void getDeviceName(){ Particle.publish("particle/device/name", NULL, 60, PRIVATE); Serial.print("wait"); }
inline void startFlush(){ MixMaster.startFlush(); }
inline void printCurrentPressure(){ Serial.print(PressureManager.getPressure()); }
inline void printPailState(){ Serial.print(PailSensor.getState()); }
inline void printPailRaw(){ Serial.print(PailSensor.getCurrentReading()); }
inline void updatePailThreshold(){ PailSensor.setDetectionThreshold(settings.pailThreshold); }
inline void printIsPrimed(){ Serial.print(isPrimed); }
inline void printIncrementer(){ Serial.print(debug_incrementer); }
inline void updateChargePressure() { PressureManager.updateTargetPressure(settings.chargePressure); }
inline void updateChargeTimeout() { PressureManager.updateChargeTimeout(settings.chargeTimeout); }
inline void updateChargeCoolDown() { PressureManager.updateChargeCoolDown(settings.chargeCoolDown); }

// Command Declarations
typedef struct {
  const char* name;
  uint32_t* setting;
  bool isSelectorConfigured;
  void (*action)(void);
} commandSet;

// List of serial commands
// Form of: {commandString, settingStructMember, bool isSelectorConfigured, function to call}
//    - for commands referncing the  "settings" struct: bool isSelectorConfigured
//          may or many not be true and action function is typically NULL
//          * if the setting's NOT selectorConfigured, pass by reference (ex. {"eepromVersion", &settings.version, false, NULL})
//          * if the setting IS selectorConfigured, you are passing an array by reference (ex. {"flowRate", settings.flowRate, true, NULL})
//    - for commands NOT reference the "settings struct": *setting and bool isSelectorConfigured
//          should be NULL. All command actions should take place in the command function
//          defined above
//    - List must be end with {NULL,NULL,NULL,NULL}
const commandSet commandList[] = {
  {"action", NULL, NULL, printMixerState},
  {"autoReverseSteps", &settings.autoReverseSteps, false, NULL},
  {"cloudStatus", NULL, NULL, printCloudStatus},
  {"chargePressure", &settings.chargePressure, false, updateChargePressure},
  {"chargeTimeout", &settings.chargeTimeout, false, updateChargeTimeout},
  ("chargeCoolDown", &settings.chargeCoolDown, false, updateChargeCoolDown),
  {"currentPressure", NULL, NULL, printCurrentPressure},
  {"error", NULL, NULL, printCurrentError},
  {"eepromVersion", &settings.version, false, NULL},
  {"firmwareID", NULL, NULL, printFirmwareID},
  {"flowRate", settings.flowRate, true, NULL},
  {"incrementer", NULL, NULL, printIncrementer},
  {"keepOpenVolume", &settings.keepOpenVolume, false, NULL},
  {"resinPumpSpeed", NULL, NULL, printResinPumpSpeed},
  {"hardenerPumpSpeed", NULL, NULL, printHardenerPumpSpeed},
  {"maxNoPressure", &settings.maxNoPressure, false, NULL},
  {"minChargingTime", &settings.minChargingTime, false, NULL},
  {"minPrimes", &settings.minPrimes, false, NULL},
  {"name", NULL, NULL, getDeviceName},
  {"numSelectors", NULL, NULL, printNumSelectors},
  {"pailThreshold", &settings.pailThreshold, false, updatePailThreshold},
  {"pailState", NULL, NULL, printPailState},
  {"pailRaw", NULL, NULL, printPailRaw},
  {"primeVolume", &settings.primeVolume, false, NULL},
  {"primeState", NULL, NULL, printIsPrimed},
  {"ratioResin", settings.ratioResin, true, NULL},
  {"ratioHardener", settings.ratioHardener, true, NULL},
  {"selector", NULL, NULL, printCurrentSelector},
  {"startFlush", NULL, NULL, startFlush},
  {"stepsPerMlResin", &settings.stepsPerMlResin, false, NULL},
  {"stepsPerMlHardener", &settings.stepsPerMlHardener, false, NULL},
  {"togglePump", NULL, NULL, togglePump},
  {"version", NULL, NULL, printProductVersion},
  {"volume", settings.volume, true, NULL},
  {"wifiStatus", &wifiStatus, false, NULL },
  {NULL, NULL, NULL, NULL}
  };

void readSerial(char c);

#endif
