/*
 * Project: Mix Controller
 * Description:
 * Author: Ben Veenema
 * Date: 3/8/18
 */

#include "Particle.h"
#include "config.h"
#include "globals.h"
#include <clickButton.h>
#include "AppInterface.h"
#include "MixMaster.h"
#include "StatusLED.h"
#include "LiquidSensor.h"
#include "PailSensor.h"

PRODUCT_ID(THIS_PRODUCT_ID);
PRODUCT_VERSION(THIS_PRODUCT_VERSION);

SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(SEMI_AUTOMATIC);

ClickButton Button(BUTTON_PIN, LOW, CLICKBTN_PULLUP);
ClickButton Remote(REMOTE_PIN, HIGH);

void setup() {
  EEPROM.get(settingsAddr, settings);
  if(settings.version != THIS_EEPROM_VERSION) {
    // Memory was not previously set or format updated, initialize
    settings = defaultSettings;
  }

  // Create system even for reset and update (make sure motors stop);
  System.on(reset+firmware_update, fwUpdateAndResetHandler);

  // Initializations (Serial and IOExp **MUST** be first)
  Serial.begin(57600); // baud 57600
  IOExp.begin(0); // Address 0
  MixMaster.init();
  StatusLED.init();
  PailSensor.init();
  PailSensor.setDetectionThreshold(settings.pailThreshold);
  ResinLiquidSensor.init(LIQUID_SNS_RESIN_IOEXP_PIN);
  HardenerLiquidSensor.init(LIQUID_SNS_HARDENER_IOEXP_PIN);

  Remote.debounceTime = 10;
  Button.longClickTime = LONG_PRESS_TIME;
  Remote.longClickTime = LONG_PRESS_TIME;

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(REMOTE_PIN, INPUT_PULLDOWN);
  IOExp.pinMode(ROTARY_1_IOEXP_PIN, INPUT);
  IOExp.pullUp(ROTARY_1_IOEXP_PIN, HIGH);
  IOExp.pinMode(ROTARY_2_IOEXP_PIN, INPUT);
  IOExp.pullUp(ROTARY_2_IOEXP_PIN, HIGH);
  IOExp.pinMode(ROTARY_3_IOEXP_PIN, INPUT);
  IOExp.pullUp(ROTARY_3_IOEXP_PIN, HIGH);
  IOExp.pinMode(ROTARY_4_IOEXP_PIN, INPUT);
  IOExp.pullUp(ROTARY_5_IOEXP_PIN, HIGH);

  delay(100);
}

void loop() {
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

  // Update the mix Master
  //  pass the changeState bool to MixMaster, if true, MixMaster will return it
  //  as false when after it has processed the command
  changeState = MixMaster.update(changeState);

  // check buttons
  // positive value for clicks is number of short presses
  // negative value for click is number of long presses
  Button.Update();
  Remote.Update();
  if(Button.clicks > 0 || Remote.clicks > 0) changeState = true; // short press
  if(Button.clicks < 0 || Remote.clicks < 0) MixMaster.startFlush(); // long press
  if(Remote.clicks > 0) Serial.println("Remote SHORT Press");
  if(Button.clicks > 0) Serial.println("Button SHORT Press");
  if(Remote.clicks < 0) Serial.println("Remote LONG Press");
  if(Button.clicks < 0) Serial.println("Button LONG Press");


  // Update modules
  StatusLED.update();
  PailSensor.update();
  ResinLiquidSensor.update();
  HardenerLiquidSensor.update();

}

int checkSelectorSwitch() {
  static unsigned int oldSelector = 0;
  unsigned int newSelector = 0;

  if(!IOExp.digitalRead(ROTARY_1_IOEXP_PIN)){
    newSelector = 1;
  } else if(!IOExp.digitalRead(ROTARY_2_IOEXP_PIN)){
    newSelector = 2;
  } else if(!IOExp.digitalRead(ROTARY_2_IOEXP_PIN)){
    newSelector = 3;
  } else if(!IOExp.digitalRead(ROTARY_2_IOEXP_PIN)){
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

void nameHandler(const char *topic, const char *data) {
    Serial.print("name:" + String(data));
    Serial.print('\n');
}

void fwUpdateAndResetHandler(){
  pinSetFast(RESIN_PUMP_ENABLE_PIN);
  pinSetFast(HARDENER_PUMP_ENABLE_PIN);
}
