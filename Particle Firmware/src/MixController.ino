/*
 * Project: Mix Controller
 * Description:
 * Author: Ben Veenema
 * Date: 3/8/18
 */

#include "Particle.h"
#include "AccelStepper.h"
#include "Bounce.h"


#define THIS_PRODUCT_ID 1111
#define THIS_PRODUCT_VERSION 1
// PRODUCT_ID(5864);
// PRODUCT_VERSION(1);

#define MOTORA_ENABLE_PIN D0
#define MOTORA_DIR_PIN D1
#define MOTORA_STEP_PIN D2
#define MOTORA_ASSERT_PIN D3

#define MOTORB_ENABLE_PIN D4
#define MOTORB_DIR_PIN D5
#define MOTORB_STEP_PIN D6
#define MOTORB_ASSERT_PIN D7

#define BUTTON_PIN A0
#define ERROR_LED_PIN A1

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
  uint16_t stepsPerMlA;
  uint16_t stepsPerMlB;
} settings;

uint32_t settingsAddr = 0;

const uint16_t default_flowRate = 3000;
const uint16_t default_ratioA = 200;
const uint16_t default_ratioB = 100;
const uint32_t default_autoReverse = 1000;
const uint16_t default_stepsPerMlA = 66;
const uint16_t default_stepsPerMlB = 66;


AccelStepper motorA(AccelStepper::DRIVER, MOTORA_STEP_PIN, MOTORA_DIR_PIN);
AccelStepper motorB(AccelStepper::DRIVER, MOTORB_STEP_PIN, MOTORB_DIR_PIN);
uint8_t STATE_mixer = 0; //0: Not Moving 1: Mixing 2: Start AutoReverse 3: AutoReversing
uint32_t motorSpeedA = 0; //steps/s
uint32_t motorSpeedB = 0; //steps/s
bool FLAG_wasError = 0;
bool FLAG_justReset = 0;
#define ultimateMaxSpeed 15000
#define autoReverseSpeed 4000

Bounce debouncer = Bounce();


void setup() {
  Serial.begin(57600);

  EEPROM.get(settingsAddr, settings);
  if(settings.version != 0) {
    // Memory was not previously set, initialize
    prom temp = {0, default_flowRate, default_ratioA, default_ratioB,
                  default_autoReverse, default_stepsPerMlA, default_stepsPerMlB
                };
    settings = temp;
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

  delay(100);

  debouncer.attach(BUTTON_PIN);
  debouncer.interval(30);

  digitalWrite(ERROR_LED_PIN, LOW);

  digitalWrite(MOTORA_ENABLE_PIN, LOW);
  digitalWrite(MOTORB_ENABLE_PIN, LOW);

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

  debouncer.update();

  bool buttonPressed = debouncer.read();
  if(buttonPressed) STATE_mixer = 1; //Mixing

  if(STATE_mixer == 0){ // Not Running
    motorA.setSpeed(0);
    motorB.setSpeed(0);
  }else if(STATE_mixer == 1){ // Mixing
    motorA.setMaxSpeed(ultimateMaxSpeed);
    motorB.setMaxSpeed(ultimateMaxSpeed);
    motorA.setSpeed(motorSpeedA);
    motorB.setSpeed(motorSpeedB);
    motorA.runSpeed();
    motorB.runSpeed();
    if(!buttonPressed) STATE_mixer = 0;
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


void nameHandler(const char *topic, const char *data) {
    Serial.print("name:" + String(data));
    Serial.print('\n');
}

void fwUpdateAndResetHandler(){
  pinSetFast(MOTORA_ENABLE_PIN);
  pinSetFast(MOTORB_ENABLE_PIN);
}
