/*
 * Project: Mix Controller
 * Description:
 * Author: Ben Veenema
 * Date: 3/8/18
 */

#include "Particle.h"
#include "config.h"
#include "globals.h"
#include "AccelStepper.h"
#include <clickButton.h>
#include "SerialSettings.h"

PRODUCT_ID(THIS_PRODUCT_ID);
PRODUCT_VERSION(THIS_PRODUCT_VERSION);

SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(SEMI_AUTOMATIC);

AccelStepper motorA(AccelStepper::DRIVER, MOTORA_STEP_PIN, MOTORA_DIR_PIN);
AccelStepper motorB(AccelStepper::DRIVER, MOTORB_STEP_PIN, MOTORB_DIR_PIN);

ClickButton button(BUTTON_PIN, HIGH);
ClickButton remote(REMOTE_PIN, HIGH);


void setup() {
  Serial.begin(57600);

  EEPROM.get(settingsAddr, settings);
  if(settings.version != THIS_EEPROM_VERSION) {
    // Memory was not previously set, initialize
    settings = defaultSettings;
  }

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

  // Check Wifi Status Setting
  static unsigned int previousWifiStatus = 0;
  if(wifiStatus != previousWifiStatus){
    previousWifiStatus = wifiStatus;
    if(wifiStatus){
      Particle.connect();
      Particle.subscribe("particle/device/name", nameHandler, MY_DEVICES);
    }else {
      WiFi.off();
    }
  }

  // Check setting selector

  int newSelector = checkSelectorSwitch();
  if(newSelector >= 0 && selector != newSelector){
    selector = newSelector;
  }

  // State Machine
  static uint32_t timeToMix = 0;
  static uint32_t timeStartedMixing = 0;
  if(STATE_mixer == 0){ // Not Running
    motorA.setSpeed(0);
    motorB.setSpeed(0);
    digitalWrite(MOTORA_ENABLE_PIN, HIGH); // Disable Motor A
    digitalWrite(MOTORB_ENABLE_PIN, HIGH); // Disable Motor B
    if(changeState == true) { Serial.println("Changing State"); STATE_mixer = 1; }
  }else if(STATE_mixer == 1){ // Mixing Calculations
    motorSpeedA = calculateMotorSpeed(settings.flowRate[selector], settings.ratioA[selector], settings.ratioB[selector], settings.stepsPerMlA);
    motorSpeedB = calculateMotorSpeed(settings.flowRate[selector], settings.ratioB[selector], settings.ratioA[selector], settings.stepsPerMlB);
    timeToMix = calculateTimeForVolume(settings.volume[selector], settings.flowRate[selector]);
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
      if(settings.autoReverseA[selector] > 0 || settings.autoReverseB[selector] > 0) STATE_mixer = 3;
      else STATE_mixer = 0;
    }
  }else if(STATE_mixer == 3){ // Start AutoReverse
    motorA.setMaxSpeed(autoReverseSpeed);
    motorB.setMaxSpeed(autoReverseSpeed);
    motorA.setCurrentPosition(0);
    motorB.setCurrentPosition(0);
    motorA.moveTo(-settings.autoReverseA[selector]);
    motorB.moveTo(-settings.autoReverseB[selector]);
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
    serialCommandHander(variableNameBuffer, selectorBuffer, valueBuffer, FLAG_isWrite);
    valueBuffer[0] = 0;
    FLAG_messageReceived = false;
    if(FLAG_isWrite){
      FLAG_isWrite = false;
      EEPROM.put(settingsAddr, settings);
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
  else {
    oldSelector = newSelector;
    return -1;
  }
}

void serialEvent(){
  readSerial(Serial.read());
}

uint32_t calculateMotorSpeed(uint16_t flowRate, uint16_t thisMotorRatio, uint16_t otherMotorRatio, uint16_t stepsPerMl){
  if(thisMotorRatio == 0 && otherMotorRatio == 0) return 0; // prevent divide by 0 error
  uint32_t motorSpeed = flowRate*stepsPerMl*thisMotorRatio/(thisMotorRatio+otherMotorRatio)/60;
  return motorSpeed;
}

uint32_t calculateTimeForVolume(uint32_t volume, uint16_t flowRate){
  if(volume > 7158278) return 0; // Largest value before variable overflow
  if(flowRate == 0) return 0; // prevent divide by 0 error
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
