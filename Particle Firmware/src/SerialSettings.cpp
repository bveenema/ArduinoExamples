#include "SerialSettings.h"

void serialCommandHander(char* commandName, uint32_t selector, char* newValue, bool isWrite){
  Serial.print(commandName);
  Serial.print(':');

  for(int i=0; commandList[i].name != NULL; i++){
    if(strcmp(commandList[i].name, commandName) == 0){
      uint32_t tempSelector = selector;
      if(commandList[i].isSelectorConfigured) tempSelector = 0;

      // handle setting if write
      if(isWrite){
        if(commandList[i].setting) *(commandList[i].setting+tempSelector) = atoi(newValue);
      }

      // report setting back to sender
      if(commandList[i].setting) Serial.print(*commandList[i].setting);

      // handle any action function
      if(commandList[i].action) commandList[i].action();
      break;
    } else {
      Serial.print("NOT Recognized");
    }
    Serial.print("\n");
  }
}
