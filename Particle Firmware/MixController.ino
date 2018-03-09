/*
 * Project: Mix Controller
 * Description:
 * Author: Ben Veenema
 * Date: 3/8/18
 */

#include "Particle.h"


#define THIS_PRODUCT_ID 1111
#define THIS_PRODUCT_VERSION 1
// PRODUCT_ID(5864);
// PRODUCT_VERSION(1);

//SYSTEM_MODE(SEMI_AUTOMATIC);
SYSTEM_THREAD(ENABLED);

bool FLAG_messageReceived = false;
bool FLAG_isWrite = false;

const size_t messageBufferSize = 128;
char messageBuffer[messageBufferSize];
char variableNameBuffer[32];
char valueBuffer[32];
uint8_t messageIndex = 0;

struct prom {
  bool version;
  uint16_t flowRate;
  uint16_t ratioA;
  uint16_t ratioB;
  uint32_t autoReverse;
} settings;

uint32_t settingsAddr = 0;

const uint16_t default_flowRate = 400;
const uint16_t default_ratioA = 200;
const uint16_t default_ratioB = 100;
const uint32_t default_autoReverse = 10000;

void setup() {
  Serial.begin(57600);

  EEPROM.get(settingsAddr, settings);
  if(settings.version != 0) {
    // Memory was not previously set, initialize
    prom temp = {0, default_flowRate, default_ratioA, default_ratioB, default_autoReverse};
    settings = temp;
  }

  Particle.subscribe("particle/device/name", handler);

}

void loop() {
  if(FLAG_messageReceived){
    FLAG_messageReceived = false;

    Serial.print(variableNameBuffer);
    Serial.print(':');

    if(strcmp("flowRate", variableNameBuffer) == 0){
      if(FLAG_isWrite) settings.flowRate = atoi(valueBuffer);
      Serial.print(settings.flowRate);
    }else if(strcmp("ratioA", variableNameBuffer) == 0){
      if(FLAG_isWrite) settings.ratioA = atoi(valueBuffer);
      Serial.print(settings.ratioA);
    }else if(strcmp("ratioB", variableNameBuffer) == 0){
      if(FLAG_isWrite) settings.ratioB = atoi(valueBuffer);
      Serial.print(settings.ratioB);
    }else if(strcmp("autoReverse", variableNameBuffer) == 0){
      if(FLAG_isWrite) settings.autoReverse = atoi(valueBuffer);
      Serial.print(settings.autoReverse);
    }else if(strcmp("firmwareID", variableNameBuffer) == 0){
      Serial.print(THIS_PRODUCT_ID);
    }else if(strcmp("version", variableNameBuffer) == 0){
      Serial.print(THIS_PRODUCT_VERSION);
    }else if(strcmp("name", variableNameBuffer) == 0){
      Serial.print("Unknown");
      Particle.publish("particle/device/name");
    }else if(strcmp("cloudStatus", variableNameBuffer) == 0){
      if(Particle.connected()){
        Serial.print("Connected");
      }else {
        Serial.print("Not Available");
      }
    }
    else {
      Serial.print(valueBuffer);
    }
    Serial.print('\n');

    valueBuffer[0] = 0;

    if(FLAG_isWrite){
      FLAG_isWrite = false;
      EEPROM.put(settingsAddr,settings);
    }
  }

}

void serialEvent(){
  /*
  Serial data comes in form of:
    {variableName}:{value} for a write, and
    {variableName} (with no ":") for a read
  followed by a newline character.

  ex. WRITE --> flowRate:250\n
      READ  --> flowRate\n
  */


  if(messageIndex < messageBufferSize) {
    char c = Serial.read();
    if(c != '\n' && c != ':') {
      messageBuffer[messageIndex++] = c;
    }else if(c == ':'){
      FLAG_isWrite = true;
      messageBuffer[messageIndex] = 0;
      strcpy(variableNameBuffer, messageBuffer);
      messageIndex = 0;
    }else {
      messageBuffer[messageIndex] = 0;
      if(FLAG_isWrite){
        strcpy(valueBuffer, messageBuffer);
      }else{
        strcpy(variableNameBuffer, messageBuffer);
      }
      FLAG_messageReceived = true;
      messageIndex = 0;
    }
  }else {
    messageIndex = 0;
  }
}


void handler(const char *topic, const char *data) {
    Serial.print("name:" + String(data));
    Serial.print('\n');
}
