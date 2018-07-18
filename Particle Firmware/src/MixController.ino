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

const size_t messageBufferSize = 128;
char messageBuffer[messageBufferSize];
char variableNameBuffer[32];
char valueBuffer[32];
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
prom settings;


void setup() {
  Serial.begin(57600);

  EEPROM.get(settingsAddr, settings);
  if(settings.version != 0) {
    // Memory was not previously set, initialize
    settings = default_settings;
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

  motorSpeedA = calculateMotorSpeed(settings.flowRate, settings.ratioA, settings.ratioB, settings.stepsPerMlA);
  motorSpeedB = calculateMotorSpeed(settings.flowRate, settings.ratioB, settings.ratioA, settings.stepsPerMlB);
}

void loop() {

  button.Update();
  remote.Update();

  bool changeState = false;
  if(button.clicks != 0 || remote.clicks !=0) changeState = true;

  // Not Running
  // Do Mixing Calculations
  // Mixing

  static uint32_t timeToMix = 0;
  static uint32_t timeStartedMixing = 0;
  if(STATE_mixer == 0){ // Not Running
    motorA.setSpeed(0);
    motorB.setSpeed(0);
    digitalWrite(MOTORA_ENABLE_PIN, HIGH); // Disable Motor A
    digitalWrite(MOTORB_ENABLE_PIN, HIGH); // Disable Motor B
    if(changeState == true) STATE_mixer = 1;
  }else if(STATE_mixer == 1){ // Mixing Calculations
    timeToMix = calculateTimeForVolume(settings.volume, settings.flowRate);
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
    if(changeState == true) STATE_mixer = 0;
    if(millis() - timeStartedMixing > timeToMix) STATE_mixer = 0;
  }


  if(FLAG_messageReceived){
    FLAG_messageReceived = false;

    Serial.print(variableNameBuffer);
    Serial.print(':');

    if(strcmp("flowRate", variableNameBuffer) == 0){
      if(FLAG_isWrite) {
        settings.flowRate = atoi(valueBuffer);
        motorSpeedA = calculateMotorSpeed(settings.flowRate, settings.ratioA, settings.ratioB, settings.stepsPerMlA);
        motorSpeedB = calculateMotorSpeed(settings.flowRate, settings.ratioB, settings.ratioA, settings.stepsPerMlB);
      }
      Serial.print(settings.flowRate);
    }else if(strcmp("ratioA", variableNameBuffer) == 0){
      if(FLAG_isWrite) {
        settings.ratioA = atoi(valueBuffer);
        motorSpeedA = calculateMotorSpeed(settings.flowRate, settings.ratioA, settings.ratioB, settings.stepsPerMlA);
        motorSpeedB = calculateMotorSpeed(settings.flowRate, settings.ratioB, settings.ratioA, settings.stepsPerMlB);
      }
      Serial.print(settings.ratioA);
    }else if(strcmp("ratioB", variableNameBuffer) == 0){
      if(FLAG_isWrite) {
        settings.ratioB = atoi(valueBuffer);
        motorSpeedA = calculateMotorSpeed(settings.flowRate, settings.ratioA, settings.ratioB, settings.stepsPerMlA);
        motorSpeedB = calculateMotorSpeed(settings.flowRate, settings.ratioB, settings.ratioA, settings.stepsPerMlB);
      }
      Serial.print(settings.ratioB);
    }else if(strcmp("stepsPerMlA", variableNameBuffer) == 0){
      if(FLAG_isWrite) {
        settings.stepsPerMlA = atoi(valueBuffer);
        motorSpeedA = calculateMotorSpeed(settings.flowRate, settings.ratioA, settings.ratioB, settings.stepsPerMlA);
      }
      Serial.print(settings.stepsPerMlA);
    }else if(strcmp("stepsPerMlB", variableNameBuffer) == 0){
      if(FLAG_isWrite) {
        settings.stepsPerMlB = atoi(valueBuffer);
        motorSpeedB = calculateMotorSpeed(settings.flowRate, settings.ratioB, settings.ratioA, settings.stepsPerMlB);
      }
      Serial.print(settings.stepsPerMlB);
    }else if(strcmp("volume", variableNameBuffer) == 0){
      if(FLAG_isWrite) {
        settings.volume = atoi(valueBuffer);
      }
      Serial.print(settings.volume);
    }else if(strcmp("autoReverse", variableNameBuffer) == 0){
      if(FLAG_isWrite) settings.autoReverse = atoi(valueBuffer);
      Serial.print(settings.autoReverse);
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
