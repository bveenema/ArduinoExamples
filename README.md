# Mix Controller
A chrome extension user interface and embedded firmware for controlling mixing pumps.

#### Index
1. [User Interface (Chrome Extension)](#user-interface)
  * [Installation](#installation)
  * [Create Shortcuts](#create-shortcuts)
  * [How to Use](#how-to-use)
* [Firmware (Mixer Software)](#firmware)
  * [Driver Installation](#driver-installation)

## User Interface (Chrome Extension)
### Installation
To install the user interface:
1. Download and extract the zip file
![Img of where to go to download ZIP][JPG_downloadZIP]

* Open the **Extensions** page in Chrome
![Type "chrome://extensions" in the address bar][JPG_GoToExtensions]

* Turn on **Developer Mode** and **Load unpacked extension...** . Navigate to the extracted zip file and select the **Chrome Extension** folder.  Click **OK**
![Check the box next to "Developer Mode" and then click "Load unpacked extension"][JPG_LoadUnpackedExtension]

* **Mix Controller** extension should be loaded
![Mix Controller is at top of extension list][JPG_MixControllerExtension]

### Create Shortcuts
To create Desktop/Start Menu/Taskbar shortcuts:
1. Click **Details** under Mix Controller, select **Create Shortcuts...** , select or deselect the shortcuts you want to create , click **Create**
![How to create shortcuts][JPG_CreateShortcuts]

### How to Use
To update the settings on the firmware

1. Launch **Mix Controller** app/extension
* Select the device in the drop-down list
* Change settings using the sliders or text boxes
  * Use slider for quick adjustments
  * Text box will update with slider value
  * Exact numbers can be typed in the box
  * **Current** displays the value the actual controller recognizes and has stored


## Firmware (Mixer Software)
### Driver Installation





[JPG_downloadZIP]: ReadmeAssets/DownloadZip.jpg "How to download ZIP"

[JPG_GoToExtensions]: ReadmeAssets/GoToExtensions.jpg "Or type \"chrome://extensions\" in the address bar"

[JPG_LoadUnpackedExtension]: ReadmeAssets/LoadUnpackedExtension.jpg "Check the box next to \"Developer Mode\" and then click \"Load unpacked extension\""

[JPG_MixControllerExtension]: ReadmeAssets/MixControllerExtension.jpg "Mix Controller Extension"

[JPG_CreateShortcuts]: ReadmeAssets/CreateShortcuts.jpg "How to create shortcuts"
