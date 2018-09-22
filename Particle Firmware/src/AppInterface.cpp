#include "AppInterface.h"

// Variables for handling serial protocol
bool FLAG_isWrite = false;
bool FLAG_isSelectorSetting = false;
const size_t messageBufferSize = 128;
char messageBuffer[messageBufferSize];
char variableNameBuffer[32];
char valueBuffer[32];
uint32_t selectorBuffer = 0;
uint8_t messageIndex = 0;

void serialCommandHander(char* commandName, uint32_t selector, char* newValue, bool isWrite){
  Serial.print(commandName);
  Serial.print(':');

  bool foundCommand = false;
  for(int i=0; commandList[i].name != NULL; i++){
    if(strcmp(commandList[i].name, commandName) == 0){
      uint32_t tempSelector = selector;
      if(!commandList[i].isSelectorConfigured) tempSelector = 0;

      // handle setting if write
      if(isWrite){
        if(commandList[i].setting) *(commandList[i].setting+tempSelector) = atoi(newValue);
      }

      // report setting back to sender
      if(commandList[i].setting) Serial.print(*(commandList[i].setting+tempSelector));

      // handle any action function
      if(commandList[i].action) commandList[i].action();

      foundCommand = true;
      break;
    }
  }
  if(!foundCommand) Serial.print("NOT Recognized");

  Serial.print("\n");
}

void readSerial(char c){
  /*
  Serial data comes in form of:
    {variableName}:{selector}:{value} for a write, and
    {variableName}:{selector} (with no ":") for a read
  followed by a newline character.
  **if a variable not requiring a selector (ex. stepsPerMlResin), then {selector}
    can be any value and is ignored by the interpreter

  ex. WRITE --> flowRate:1:250
      READ  --> flowRate:1
  */
  if(messageIndex >= messageBufferSize) messageIndex = 0;

  if(c != '\n' && c != ':') {
    messageBuffer[messageIndex++] = c;
  }else if(c == ':'){
    messageBuffer[messageIndex] = 0;
    if(FLAG_isSelectorSetting) {
      selectorBuffer = atoi(messageBuffer); // Data was Selector Number
      FLAG_isWrite = true; // Next data is value to write
    } else {
      strcpy(variableNameBuffer, messageBuffer); // Data was variableName
      FLAG_isSelectorSetting = true; // Next data is selector setting
    }
    messageIndex = 0;
  }else {
    messageBuffer[messageIndex] = 0;
    if(FLAG_isWrite){
      strcpy(valueBuffer, messageBuffer); // Data was new value
    } else if(FLAG_isSelectorSetting){
      selectorBuffer = atoi(messageBuffer); // Data was Selector Number
    } else{
      selectorBuffer = 0; // No Selector Setting was sent
      strcpy(variableNameBuffer, messageBuffer); // Data was variable name
    }
    FLAG_isSelectorSetting = false;
    messageIndex = 0;

    // Interpet the completed command
    serialCommandHander(variableNameBuffer, selectorBuffer, valueBuffer, FLAG_isWrite);
    valueBuffer[0] = 0;
    if(FLAG_isWrite){
      FLAG_isWrite = false;
      EEPROM.put(settingsAddr, settings);
    }
  }
}
