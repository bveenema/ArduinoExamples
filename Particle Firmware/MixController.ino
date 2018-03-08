/*
 * Project: Mix Controller
 * Description:
 * Author: Ben Veenema
 * Date: 3/8/18
 */

#include "Particle.h"

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

uint16_t flowRate = 400;
uint16_t ratioA = 200;
uint16_t ratioB = 100;
uint32_t autoReverse = 10000;

void setup() {
  Serial.begin(57600);

  delay(500);

}

void loop() {
  if(FLAG_messageReceived){
    FLAG_messageReceived = false;
    Serial.print(variableNameBuffer);
    Serial.print(':');
    if(strcmp("flowRate", variableNameBuffer) == 0){
      if(FLAG_isWrite) flowRate = atoi(valueBuffer);
      Serial.print(flowRate);
    }else if(strcmp("ratioA", variableNameBuffer) == 0){
      if(FLAG_isWrite) ratioA = atoi(valueBuffer);
      Serial.print(ratioA);
    }else if(strcmp("ratioB", variableNameBuffer) == 0){
      if(FLAG_isWrite) ratioB = atoi(valueBuffer);
      Serial.print(ratioB);
    }else if(strcmp("autoReverse", variableNameBuffer) == 0){
      if(FLAG_isWrite) autoReverse = atoi(valueBuffer);
      Serial.print(autoReverse);
    } else {
      Serial.print(valueBuffer);
    }
    Serial.print('\n');

    if(FLAG_isWrite){
      FLAG_isWrite = false;
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
