# Mix Controller
A chrome extension user interface and embedded firmware for controlling mixing pumps.

#### Index
1. User Interface (Chrome Extension)
  * [Installation](#installation)
  * [Create Shortcuts](#create-shortcuts)
  * [How to Use](#how-to-use)
1. Firmware (Mixer Software)
  * [Driver Installation](#driver-installation)

## User Interface (Chrome Extension)
### Installation
To install the user interface:
1. Download and extract the zip file
![Img of where to go to download ZIP][JPG_downloadZIP]

1. Open the **Extensions** page in Chrome
![Type "chrome://extensions" in the address bar][JPG_GoToExtensions]

1. Turn on **Developer Mode** and **Load unpacked extension...** . Navigate to the extracted zip file and select the **Chrome Extension** folder.  Click **OK**
![Check the box next to "Developer Mode" and then click "Load unpacked extension"][JPG_LoadUnpackedExtension]

1. **Mix Controller** extension should be loaded
![Mix Controller is at top of extension list][JPG_MixControllerExtension]

### Create Shortcuts
To create Desktop/Start Menu/Taskbar shortcuts:
1. Click **Details** under Mix Controller, select **Create Shortcuts...** , select or deselect the shortcuts you want to create , click **Create**
![How to create shortcuts][JPG_CreateShortcuts]

### How to Use
To update the settings on the firmware

1. Launch **Mix Controller** app/extension
1. Select the device in the drop-down list
1. Change settings using the sliders or text boxes
  * Use slider for quick adjustments
  * Text box will update with slider value
  * Exact numbers can be typed in the box
  * **Current** displays the value the actual controller recognizes and has stored


## Firmware (Mixer Software)
### Driver Installation
In order to communicate with the Mixer Software, the drivers must be installed on first. Installing drivers is done by installing Particle CLI, a package of software that includes Particle drivers (which the Mixer Software runs on)

###### For Mac or Linux
---

Open Terminal, or preferred terminal program and paste this command to install the Particle CLI:

``` terminal
bash <( curl -sL https://particle.io/install-cli )
```

You may need to open a new terminal after the install completes to use the particle command.

If you get a message about installing dfu-util for your OS, make sure you have [homebrew](https://brew.sh/) installed and run the command above again.

###### For Windows
---
Download the [Windows CLI Installer](https://binaries.particle.io/cli/installer/windows/ParticleCLISetup.exe) and run it to install the Particle CLI, the device drivers and the dependencies that the CLI needs.





[JPG_downloadZIP]: ReadmeAssets/DownloadZip.jpg "How to download ZIP"

[JPG_GoToExtensions]: ReadmeAssets/GoToExtensions.jpg "Or type \"chrome://extensions\" in the address bar"

[JPG_LoadUnpackedExtension]: ReadmeAssets/LoadUnpackedExtension.jpg "Check the box next to \"Developer Mode\" and then click \"Load unpacked extension\""

[JPG_MixControllerExtension]: ReadmeAssets/MixControllerExtension.jpg "Mix Controller Extension"

[JPG_CreateShortcuts]: ReadmeAssets/CreateShortcuts.jpg "How to create shortcuts"
