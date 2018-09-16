# Use of Mix Controller App

## How to Launch App
1. Goto the apps menu on your chrome browser.
2. Click on the "Mix Controller" Icon.

![Img of how to launch app][JPG_launch]

*if the app is not available, see [main readme](https://github.com/bveenema/MixController/blob/master/README.md#user-interface-chrome-extension) on installing.*

## Making Sure Device is Connected
1. When the app is initially launched and/or when a device is not connected to the computer, the "Port" drop-downb will be empty and the inputs will be disabled.  
  - If a device is connected, but the inputs are disabled, click the "Refresh" button.
  - If the "Refresh" button still does not detect the device, make sure the device is connected, then close the app, disconnect the device, relaunch the app, then re-connect the device.

![Img of Refresh button][JPG_Refresh]

## Updating Selector Based Values
Some devices have a selector switch that can change mix settings to multiple, pre-defined settings by moving the switch. When these devices are connected to the app, the app will automaticaly re-configure to provide an interface.

1. If a device with a selector switch is connected, a "Selector" drop-down will appear beneath the "Status" box.
  - if it does not appear, and there is a device with a selector switch connected, click the "Refresh" button
2. Select the "Selector" number you want to edit from the drop-down
  - The settings below will update with the current settings for the selector number
  - Clicking the "Advanced" button will show Advanced settings. These are also tied to the selector number (except **Steps/ml - A** and **Steps/ml - B** which are the same for all selector numbers)
  ![Img of selector drop-down][JPG_Selector]
3. Update the Settings
  - Settings can be updated by moving the sliders or typing new values in the boxes
  - Settings can be updated while the motors are mixing however, new settings will not take effect until the motors are re-started
  - When a setting is updated the number next to **Current** for the corresponding setting should update to the new number. If this does not happen, there is a connection issue with the device.
  ![Img of current values][JPG_currentValues]


**IMPORTANT** - The value of the *Selector* dropdown has **nothing** to do with the selector switch. It is only for updating the configuration at each selector position.  Selector position can only be update by manually moving the switch. The current value of the selector switch can be seen in the "Controller State" section.
![Img of Selector Postion][JPG_currentSelectorSwitchPosition]

## Special Buttons
Some devices can controlled throught the app to turn the mixing motors on or off and/or have their Wifi radios enabled.  When these devices are connected, the app will automatically reconfigure to allow these interactions.

* Toggle Pumps
  - Clicking the **Toggle Pumps** button will turn the mixers on or off
  - *Action* will be "0" when the motors are **off** and "2" when the motors are **on**
  ![Img of Toggle Pumps Button][JPG_togglePumps]

* Enable Wifi
  - The Wifi radio is off by default and every time the device is restarted or power cycled
  - Clicking **Enable Wifi** will turn the wifi module on and start the process of connecting
  - Once the Wifi has successfully connected to the cloud, the app will update the *Name* and *Cloud Status* values in the *Controller State* section.
  - **Enable Wifi** will only connect to the cloud if the device has been previously connected to the local wifi network.  All devices have been configured to connect to *McNamara Design* wifi network.  [Setting up the wifi](https://docs.particle.io/guide/getting-started/start/photon/#connect-your-photon) credentials is beyond the scope of this document.
  ![Img of Enable Wifi Button][JPG_enableWifi]


[JPG_launch]: readmeAssets/Launch.jpg "How to Launch App"
[JPG_Refresh]: readmeAssets/Refresh.jpg "Refresh button"
[JPG_Selector]: readmeAssets/Selector.jpg "Selector Drow-down"
[JPG_currentValues]: readmeAssets/currentValues.jpg "Current Values"
[JPG_currentSelectorSwitchPosition]: readmeAssets/currentSelectorSwitchPosition.jpg "Current Selector Switch Position"
[JPG_togglePumps]: readmeAssets/togglePumps.jpg "Toggle Pumps"
[JPG_enableWifi]: readmeAssets/enableWifi.jpg "Enable Wifi"
