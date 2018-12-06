#ifndef AppInterface_h
#define AppInterface_h


#include "Particle.h"
#include "config.h"

// Link to external variables
#include "globals.h"
#include "PailSensor.h"
#include "PressureManager.h"
#include "Chime.h"
#include "TemperatureMonitor.h"
#include "MixMaster.h"

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
inline void updateOnPressure() { PressureManager.updateTargetPressure(settings.onPressure,1); }
inline void updateOffPressure() { PressureManager.updateTargetPressure(settings.offPressure,0); }
inline void silenceChime() { Chime.silence(); }
inline void setTemperatureRange() { ResinTemp.setRange(settings.minTemperature,settings.maxTemperature); HardenerTemp.setRange(settings.minTemperature,settings.maxTemperature); }
inline void getResinTemperature() { Serial.print(ResinTemp.getTemp()); }
inline void getHardenerTemperature() { Serial.print(HardenerTemp.getTemp()); }
inline void setMaxTimeNoLiquid() { ResinLiquidSensor.setMaxTimeNoLiquid(settings.maxTimeNoLiquid); HardenerLiquidSensor.setMaxTimeNoLiquid(settings.maxTimeNoLiquid); }

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
  {"chargeDelay", &settings.chargeDelay, false, NULL},
  {"chargeTimeout", &settings.chargeTimeout, false, NULL},
  {"cloudStatus", NULL, NULL, printCloudStatus},
  {"currentPressure", NULL, NULL, printCurrentPressure},
  {"error", NULL, NULL, printCurrentError},
  {"eepromVersion", &settings.version, false, NULL},
  {"firmwareID", NULL, NULL, printFirmwareID},
  {"flowRate", settings.flowRate, true, NULL},
  {"flushForwardPause", &settings.flushForwardPause, false, NULL},
  {"flushForwardSteps", &settings.flushForwardSteps, false, NULL},
  {"flushReversePause", &settings.flushReversePause, false, NULL},
  {"flushReverseSteps", &settings.flushReverseSteps, false, NULL},
  {"flushRPM", &settings.flushRPM, false, NULL},
  {"flushTime", &settings.flushTime, false, NULL},
  {"incrementer", NULL, NULL, printIncrementer},
  {"keepOpenVolume", &settings.keepOpenVolume, false, NULL},
  {"resinPumpSpeed", NULL, NULL, printResinPumpSpeed},
  {"hardenerPumpSpeed", NULL, NULL, printHardenerPumpSpeed},
  {"maxNoPressure", &settings.maxNoPressure, false, NULL},
  {"maxPreMixChargingTime", &settings.maxPreMixChargingTime, false, NULL},
  {"maxTemperature", &settings.maxTemperature, false, setTemperatureRange},
  {"maxTimeNoLiquid", &settings.maxTimeNoLiquid, false, setMaxTimeNoLiquid},
  {"minPrimes", &settings.minPrimes, false, NULL},
  {"minTemperature", &settings.minTemperature, false, setTemperatureRange},
  {"name", NULL, NULL, getDeviceName},
  {"numSelectors", NULL, NULL, printNumSelectors},
  {"onPressure", &settings.onPressure, false, updateOnPressure},
  {"offPressure", &settings.offPressure, false, updateOffPressure},
  {"pailThreshold", &settings.pailThreshold, false, updatePailThreshold},
  {"pailState", NULL, NULL, printPailState},
  {"pailRaw", NULL, NULL, printPailRaw},
  {"primeVolume", &settings.primeVolume, false, NULL},
  {"primeState", NULL, NULL, printIsPrimed},
  {"ratioResin", settings.ratioResin, true, NULL},
  {"ratioHardener", settings.ratioHardener, true, NULL},
  {"selector", NULL, NULL, printCurrentSelector},
  {"silence", NULL, NULL, silenceChime},
  {"startFlush", NULL, NULL, startFlush},
  {"stepsPerMlResin", &settings.stepsPerMlResin, false, NULL},
  {"stepsPerMlHardener", &settings.stepsPerMlHardener, false, NULL},
  {"temperatureHardener", NULL, NULL, getHardenerTemperature},
  {"temperatureResin", NULL, NULL, getResinTemperature},
  {"togglePump", NULL, NULL, togglePump},
  {"version", NULL, NULL, printProductVersion},
  {"volume", settings.volume, true, NULL},
  {"wifiStatus", &wifiStatus, false, NULL },
  {"zeroDripInterval", &settings.zeroDripInterval, false, NULL},
  {"zeroDripSteps", &settings.zeroDripSteps, false, NULL},
  {NULL, NULL, NULL, NULL}
  };

void readSerial(char c);

#endif
