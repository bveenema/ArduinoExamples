/*
 * Project: Mix Controller
 * Description:
 * Author: Ben Veenema
 * Date: 3/8/18
 */

#include "Particle.h"
#include "config.h"
#include "AccelStepper.h"
#include <clickButton.h>

SYSTEM_THREAD(ENABLED);

bool FLAG_messageReceived = false;
bool FLAG_isWrite = false;
bool FLAG_isSelectorSetting = false;

const size_t messageBufferSize = 128;
char messageBuffer[messageBufferSize];
char variableNameBuffer[32];
char valueBuffer[32];
uint32_t selectorBuffer;
uint8_t messageIndex = 0;


AccelStepper motorA(AccelStepper::DRIVER, MOTORA_STEP_PIN, MOTORA_DIR_PIN);
AccelStepper motorB(AccelStepper::DRIVER, MOTORB_STEP_PIN, MOTORB_DIR_PIN);
uint8_t STATE_mixer = 0; //0: Not Moving 1: Mixing 2: Start AutoReverse 3: AutoReversing
uint32_t motorSpeedA = 0; //steps/s
uint32_t motorSpeedB = 0; //steps/s
bool FLAG_wasError = 0;
bool FLAG_justReset = 0;
#define ultimateMaxSpeed 15000
#define autoReverseSpeed 4000

ClickButton button(BUTTON_PIN, HIGH);
ClickButton remote(REMOTE_PIN, HIGH);
prom_SelectorSettings settings[5];
prom_MotorSettings motorSettings;
prom_AllSettings AllSettings;


void setup() {
  Serial.begin(57600);

  EEPROM.get(settingsAddr, AllSettings);
  if(AllSettings.version != 0) {
    // Memory was not previously set, initialize
    prom_AllSettings tempAllSettings;
    tempAllSettings.version = 0;
    for(int i=0; i<NUM_SELECTORS; i++){
      tempAllSettings.selectorSettings[i] = defaultSelectorSettings;
    }
    tempAllSettings.motorSettings=defaultMotorSettings;

    AllSettings = tempAllSettings;
    EEPROM.put(settingsAddr, AllSettings);
  }
  for(int i=0; i<NUM_SELECTORS; i++){
    settings[i] = AllSettings.selectorSettings[i];
  }

  Particle.subscribe("particle/device/name", nameHandler);
  System.on(reset+firmware_update, fwUpdateAndResetHandler);

  pinMode(MOTORA_ENABLE_PIN, OUTPUT);
  pinMode(MOTORA_STEP_PIN, OUTPUT);
  pinMode(MOTORA_DIR_PIN, OUTPUT);
  pinMode(MOTORA_ASSERT_PIN, INPUT_PULLDOWN);

  pinMode(MOTORB_ENABLE_PIN, OUTPUT);
  pinMode(MOTORB_STEP_PIN, OUTPUT);
  pinMode(MOTORB_DIR_PIN, OUTPUT);
  pinMode(MOTORB_ASSERT_PIN, INPUT_PULLDOWN);

  pinMode(BUTTON_PIN, INPUT);
  pinMode(ERROR_LED_PIN, OUTPUT);
  pinMode(REMOTE_PIN, INPUT);

  pinMode(SELECTOR_SWITCH_1, INPUT_PULLDOWN);
  pinMode(SELECTOR_SWITCH_2, INPUT_PULLDOWN);
  pinMode(SELECTOR_SWITCH_3, INPUT_PULLDOWN);
  pinMode(SELECTOR_SWITCH_4, INPUT_PULLDOWN);


  remote.debounceTime = 10;

  delay(100);

  digitalWrite(ERROR_LED_PIN, LOW);

  digitalWrite(MOTORA_ENABLE_PIN, LOW); // Enable Motor A
  digitalWrite(MOTORB_ENABLE_PIN, LOW); // Enable Motor B

  motorA.setAcceleration(100000);
  motorB.setAcceleration(100000);

  motorA.setPinsInverted(0,0,1);
  motorB.setPinsInverted(0,0,1);

  motorA.setMaxSpeed(ultimateMaxSpeed);
  motorB.setMaxSpeed(ultimateMaxSpeed);
}

void loop() {

  button.Update();
  remote.Update();

  static bool changeState = false;

  // Check setting selector
  static int currentSelector = 0;
  int selector = checkSelectorSwitch();
  if(selector >= 0 && selector != currentSelector){
    currentSelector = selector;
  }

  // State Machine
  static uint32_t timeToMix = 0;
  static uint32_t timeStartedMixing = 0;
  if(STATE_mixer == 0){ // Not Running
    motorA.setSpeed(0);
    motorB.setSpeed(0);
    digitalWrite(MOTORA_ENABLE_PIN, HIGH); // Disable Motor A
    digitalWrite(MOTORB_ENABLE_PIN, HIGH); // Disable Motor B
    if(changeState == true) STATE_mixer = 1;
  }else if(STATE_mixer == 1){ // Mixing Calculations
    motorSpeedA = calculateMotorSpeed(settings[selector].flowRate, settings[selector].ratioA, settings[selector].ratioB, motorSettings.stepsPerMlA);
    motorSpeedB = calculateMotorSpeed(settings[selector].flowRate, settings[selector].ratioB, settings[selector].ratioA, motorSettings.stepsPerMlB);
    timeToMix = calculateTimeForVolume(settings[selector].volume, settings[selector].flowRate);
    timeStartedMixing = millis();
    digitalWrite(MOTORA_ENABLE_PIN, LOW); // Enable Motor A
    digitalWrite(MOTORB_ENABLE_PIN, LOW); // Enable Motor B
    STATE_mixer = 2;
  }else if(STATE_mixer == 2){ // Mixing
    motorA.setMaxSpeed(ultimateMaxSpeed);
    motorB.setMaxSpeed(ultimateMaxSpeed);
    motorA.setSpeed(motorSpeedA);
    motorB.setSpeed(motorSpeedB);
    motorA.runSpeed();
    motorB.runSpeed();
    if(changeState == true || (millis() - timeStartedMixing > timeToMix)){
      if(settings[selector].autoReverseA > 0 || settings[selector].autoReverseB > 0) STATE_mixer = 3;
      else STATE_mixer = 0;
    }
  }else if(STATE_mixer == 3){ // Start AutoReverse
    motorA.setMaxSpeed(autoReverseSpeed);
    motorB.setMaxSpeed(autoReverseSpeed);
    motorA.setCurrentPosition(0);
    motorB.setCurrentPosition(0);
    motorA.moveTo(-settings[selector].autoReverseA);
    motorB.moveTo(-settings[selector].autoReverseB);
    STATE_mixer = 4;
  }else if(STATE_mixer == 4){ // AutoReversing
    motorA.run();
    motorB.run();
    if(!motorA.isRunning() && !motorB.isRunning()){
      STATE_mixer = 0;
    }
  }

  // check buttons
  changeState = false;
  if(button.clicks != 0 || remote.clicks !=0) changeState = true;
  if(remote.clicks != 0) Serial.println("Remote Pressed");
  if(button.clicks != 0) Serial.println("Button Pressed");

  if(FLAG_messageReceived){
    FLAG_messageReceived = false;

    Serial.print(variableNameBuffer);
    Serial.print(':');

    if(strcmp("flowRate", variableNameBuffer) == 0){
      if(FLAG_isWrite) {
        settings[selectorBuffer].flowRate = atoi(valueBuffer);
      }
      Serial.print(settings[selectorBuffer].flowRate);
    }else if(strcmp("ratioA", variableNameBuffer) == 0){
      if(FLAG_isWrite) {
        settings[selectorBuffer].ratioA = atoi(valueBuffer);
      }
      Serial.print(settings[selectorBuffer].ratioA);
    }else if(strcmp("ratioB", variableNameBuffer) == 0){
      if(FLAG_isWrite) {
        settings[selectorBuffer].ratioB = atoi(valueBuffer);
      }
      Serial.print(settings[selectorBuffer].ratioB);
    }else if(strcmp("stepsPerMlA", variableNameBuffer) == 0){
      if(FLAG_isWrite) {
        motorSettings.stepsPerMlA = atoi(valueBuffer);
      }
      Serial.print(motorSettings.stepsPerMlA);
    }else if(strcmp("stepsPerMlB", variableNameBuffer) == 0){
      if(FLAG_isWrite) {
        motorSettings.stepsPerMlB = atoi(valueBuffer);
      }
      Serial.print(motorSettings.stepsPerMlB);
    }else if(strcmp("volume", variableNameBuffer) == 0){
      if(FLAG_isWrite) {
        settings[selectorBuffer].volume = atoi(valueBuffer);
      }
      Serial.print(settings[selectorBuffer].volume);
    }else if(strcmp("autoReverseA", variableNameBuffer) == 0){
      if(FLAG_isWrite) settings[selectorBuffer].autoReverseA = atoi(valueBuffer);
      Serial.print(settings[selectorBuffer].autoReverseA);
    }else if(strcmp("autoReverseB", variableNameBuffer) == 0){
        if(FLAG_isWrite) settings[selectorBuffer].autoReverseB = atoi(valueBuffer);
        Serial.print(settings[selectorBuffer].autoReverseB);
    }else if(strcmp("firmwareID", variableNameBuffer) == 0){
      Serial.print(THIS_PRODUCT_ID);
    }else if(strcmp("version", variableNameBuffer) == 0){
      Serial.print(THIS_PRODUCT_VERSION);
    }else if(strcmp("name", variableNameBuffer) == 0){
      Serial.print("wait");
      Particle.publish("particle/device/name");
    }else if(strcmp("cloudStatus", variableNameBuffer) == 0){
      if(Particle.connected()){
        Serial.print("Connected");
      }else {
        Serial.print("Not Available");
      }
    }else if(strcmp("motorSpeedA", variableNameBuffer) == 0){
      Serial.print(motorSpeedA);
    }else if(strcmp("motorSpeedB", variableNameBuffer) == 0){
      Serial.print(motorSpeedB);
    }else if(strcmp("action", variableNameBuffer) == 0){
      Serial.print(STATE_mixer);
    }else if(strcmp("toggleMotor", variableNameBuffer) == 0){
      changeState = true;
    }else if(strcmp("selector", variableNameBuffer) == 0){
      Serial.print(selector);
    }
    else {
      Serial.print(valueBuffer);
    }
    Serial.print('\n');

    valueBuffer[0] = 0;

    if(FLAG_isWrite){
      FLAG_isWrite = false;

      prom_AllSettings tempAllSettings;
      tempAllSettings.version = 0;
      for(int i=0; i<NUM_SELECTORS; i++){
        tempAllSettings.selectorSettings[i] = settings[i];
      }
      tempAllSettings.motorSettings=motorSettings;

      EEPROM.put(settingsAddr, tempAllSettings);
    }
  }

}

int checkSelectorSwitch() {
  static unsigned int oldSelector = 0;
  unsigned int newSelector = 0;

  if(pinReadFast(SELECTOR_SWITCH_1)){
    newSelector = 1;
  } else if(pinReadFast(SELECTOR_SWITCH_2)){
    newSelector = 2;
  } else if(pinReadFast(SELECTOR_SWITCH_3)){
    newSelector = 3;
  } else if(pinReadFast(SELECTOR_SWITCH_4)){
    newSelector = 4;
  } else {
    newSelector = 0;
  }

  if(newSelector == oldSelector) return newSelector;
  else return -1;
}

void serialEvent(){
  /*
  Serial data comes in form of:
    {variableName}:{selector}:{value} for a write, and
    {variableName}:{selector} (with no ":") for a read
  followed by a newline character.
  **if a variable not requiring a selector (ex. stepsPerMlA), then {selector}
    can be any value and is ignored by the interpreter

  ex. WRITE --> flowRate:1:250
      READ  --> flowRate:1
  */


  if(messageIndex < messageBufferSize) {
    char c = Serial.read();
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
      FLAG_messageReceived = true;
      messageIndex = 0;
    }
  }else {
    messageIndex = 0;
  }
}

uint32_t calculateMotorSpeed(uint16_t flowRate, uint16_t thisMotorRatio, uint16_t otherMotorRatio, uint16_t stepsPerMl){
  uint32_t motorSpeed = flowRate*stepsPerMl*thisMotorRatio/(thisMotorRatio+otherMotorRatio)/60;
  return motorSpeed;
}

uint32_t calculateTimeForVolume(uint32_t volume, uint16_t flowRate){
  if(volume > 7158278) return 0; // Largest value before variable overflow
  uint32_t time = volume*600/flowRate;
  return time * 100;
}

void nameHandler(const char *topic, const char *data) {
    Serial.print("name:" + String(data));
    Serial.print('\n');
}

void fwUpdateAndResetHandler(){
  pinSetFast(MOTORA_ENABLE_PIN);
  pinSetFast(MOTORB_ENABLE_PIN);
}
