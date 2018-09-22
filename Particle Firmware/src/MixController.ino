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
#include "AppInterface.h"

PRODUCT_ID(THIS_PRODUCT_ID);
PRODUCT_VERSION(THIS_PRODUCT_VERSION);

SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(SEMI_AUTOMATIC);

AccelStepper motorResin(AccelStepper::DRIVER, MOTOR_RESIN_STEP_PIN, MOTOR_RESIN_DIR_PIN);
AccelStepper motorHardener(AccelStepper::DRIVER, MOTOR_HARDENER_STEP_PIN, MOTOR_HARDENER_DIR_PIN);

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

  pinMode(MOTOR_RESIN_ENABLE_PIN, OUTPUT);
  pinMode(MOTOR_RESIN_STEP_PIN, OUTPUT);
  pinMode(MOTOR_RESIN_DIR_PIN, OUTPUT);
  pinMode(MOTOR_RESIN_ASSERT_PIN, INPUT_PULLDOWN);

  pinMode(MOTOR_HARDENER_ENABLE_PIN, OUTPUT);
  pinMode(MOTOR_HARDENER_STEP_PIN, OUTPUT);
  pinMode(MOTOR_HARDENER_DIR_PIN, OUTPUT);
  pinMode(MOTOR_HARDENER_ASSERT_PIN, INPUT_PULLDOWN);

  pinMode(BUTTON_PIN, INPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(REMOTE_PIN, INPUT);

  pinMode(SELECTOR_SWITCH_1, INPUT_PULLDOWN);
  pinMode(SELECTOR_SWITCH_2, INPUT_PULLDOWN);
  pinMode(SELECTOR_SWITCH_3, INPUT_PULLDOWN);
  pinMode(SELECTOR_SWITCH_4, INPUT_PULLDOWN);


  remote.debounceTime = 10;

  delay(100);

  digitalWrite(STATUS_LED_PIN, LOW);

  digitalWrite(MOTOR_RESIN_ENABLE_PIN, LOW); // Enable Motor A
  digitalWrite(MOTOR_HARDENER_ENABLE_PIN, LOW); // Enable Motor B

  motorResin.setAcceleration(100000);
  motorHardener.setAcceleration(100000);

  motorResin.setPinsInverted(0,0,1);
  motorHardener.setPinsInverted(0,0,1);

  motorResin.setMaxSpeed(ultimateMaxSpeed);
  motorHardener.setMaxSpeed(ultimateMaxSpeed);
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
    motorResin.setSpeed(0);
    motorHardener.setSpeed(0);
    digitalWrite(MOTOR_RESIN_ENABLE_PIN, HIGH); // Disable Motor A
    digitalWrite(MOTOR_HARDENER_ENABLE_PIN, HIGH); // Disable Motor B
    if(changeState == true) { changeState = false; STATE_mixer = 1; }
  }else if(STATE_mixer == 1){ // Mixing Calculations
    motorSpeedA = calculateMotorSpeed(settings.flowRate[selector], settings.ratioResin[selector], settings.ratioHardener[selector], settings.stepsPerMlResin);
    motorSpeedB = calculateMotorSpeed(settings.flowRate[selector], settings.ratioHardener[selector], settings.ratioResin[selector], settings.stepsPerMlHardener);
    timeToMix = calculateTimeForVolume(settings.volume[selector], settings.flowRate[selector]);
    timeStartedMixing = millis();
    digitalWrite(MOTOR_RESIN_ENABLE_PIN, LOW); // Enable Motor A
    digitalWrite(MOTOR_HARDENER_ENABLE_PIN, LOW); // Enable Motor B
    STATE_mixer = 2;
  }else if(STATE_mixer == 2){ // Mixing
    motorResin.setMaxSpeed(ultimateMaxSpeed);
    motorHardener.setMaxSpeed(ultimateMaxSpeed);
    motorResin.setSpeed(motorSpeedA);
    motorHardener.setSpeed(motorSpeedB);
    motorResin.runSpeed();
    motorHardener.runSpeed();
    if(changeState == true || (millis() - timeStartedMixing > timeToMix)){
      changeState = false;
      if(settings.autoReverseResin[selector] > 0 || settings.autoReverseHardener[selector] > 0) STATE_mixer = 3;
      else STATE_mixer = 0;
    }
  }else if(STATE_mixer == 3){ // Start AutoReverse
    motorResin.setMaxSpeed(autoReverseSpeed);
    motorHardener.setMaxSpeed(autoReverseSpeed);
    motorResin.setCurrentPosition(0);
    motorHardener.setCurrentPosition(0);
    motorResin.moveTo(-settings.autoReverseResin[selector]);
    motorHardener.moveTo(-settings.autoReverseHardener[selector]);
    STATE_mixer = 4;
  }else if(STATE_mixer == 4){ // AutoReversing
    motorResin.run();
    motorHardener.run();
    if(!motorResin.isRunning() && !motorHardener.isRunning()){
      STATE_mixer = 0;
    }
  }

  // check buttons
  if(button.clicks != 0 || remote.clicks !=0) changeState = true;
  if(remote.clicks != 0) Serial.println("Remote Pressed");
  if(button.clicks != 0) Serial.println("Button Pressed");

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
  if(WiFi.listening()) return;
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
  pinSetFast(MOTOR_RESIN_ENABLE_PIN);
  pinSetFast(MOTOR_HARDENER_ENABLE_PIN);
}
