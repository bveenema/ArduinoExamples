#ifndef SerialSettings_h
#define SerialSettings_h


#include "Particle.h"
#include "config.h"

// Link to external variables
//    ( variables of type other than uint32_t and Read only variables should
//      be returned via Command Function)
extern struct prom_settings settings;
extern uint32_t motorSpeedA;
extern uint32_t motorSpeedB;
extern uint8_t STATE_mixer;
extern bool changeState;
extern int selector;
extern uint32_t wifiStatus;

// Command Functions
//      ** must take no arguments and return 'void'
inline void printCurrentSelector(){ Serial.print(selector); }
inline void printFirmwareID(){ Serial.print(THIS_PRODUCT_ID); }
inline void printMixerState(){ Serial.print(STATE_mixer); }
inline void printMotorSpeedA(){ Serial.print(motorSpeedA); }
inline void printMotorSpeedB(){ Serial.print(motorSpeedB); }
inline void printNumSelectors(){ Serial.print(NUM_SELECTORS); }
inline void printProductVersion(){ Serial.print(THIS_PRODUCT_VERSION); }
inline void printCloudStatus(){
  if(Particle.connected()){
    Serial.print("Connected");
  }else {
    Serial.print("Not Available");
  }
}


inline void toggleMotor(){ changeState = true; Serial.print("N/A"); }
inline void getDeviceName(){ Particle.publish("particle/device/name", NULL, 60, PRIVATE); Serial.print("wait"); }




// Command Declarations
typedef struct {
  const char* name;
  uint32_t* setting;
  bool isSelectorConfigured;
  void (*action)(void);
} commandSet;

const commandSet commandList[] = {
  {"action", NULL, NULL, printMixerState},
  {"autoReverseA", settings.autoReverseA, true, NULL},
  {"autoReverseB", settings.autoReverseB, true, NULL},
  {"couldStatus", NULL, NULL, printCloudStatus},
  {"firmwareID", NULL, NULL, printFirmwareID},
  {"flowRate", settings.flowRate, true, NULL},
  {"motorSpeedA", NULL, NULL, printMotorSpeedA},
  {"motorSpeedB", NULL, NULL, printMotorSpeedB},
  {"name", NULL, NULL, getDeviceName},
  {"numSelectors", NULL, NULL, printNumSelectors},
  {"ratioA", settings.ratioA, true, NULL},
  {"ratioB", settings.ratioB, true, NULL},
  {"selector", NULL, NULL, printCurrentSelector},
  {"stepsPerMlA", &settings.stepsPerMlA, false, NULL},
  {"stepsPerMlB", &settings.stepsPerMlB, false, NULL},
  {"version", NULL, NULL, printProductVersion},
  {"volume", settings.volume, true, NULL},
  {"wifiStatus", &wifiStatus, false, NULL },
  {NULL, NULL, NULL, NULL}
  };


void serialCommandHander(char* commandName, uint32_t selector, char* newValue, bool isWrite);

#endif
