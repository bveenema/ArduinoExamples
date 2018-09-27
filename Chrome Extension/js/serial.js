var connectionId = -1;
var readBuffer = "";
var serialStatus = "Not Connected";

function sendMessage(message) {
  let encoder = new TextEncoder();
  let buffer = encoder.encode(message + '\n'); // add newline and encode message for transmission
  chrome.serial.send(connectionId, buffer, function(sendInfo){
    // console.log(sendInfo);
    // if(sendInfo.error) isConnected = false;
  });
};

function recieveData(readInfo) {
  let decoder = new TextDecoder();
  readBuffer += decoder.decode(readInfo.data);

  if(readBuffer.includes('\n')){
    let splitBuffer = readBuffer.split('\n');
    let messages = [];
    for(let i=0; i<splitBuffer.length-1; i++){
      messages[i] = splitBuffer[i];
    }
    readBuffer = splitBuffer[splitBuffer.length-1];

    messages.forEach(function(m){
      let message = {data:m};

      message.strings = message.data.split(':');
      message.variableName = message.strings[0];
      message.value = parseInt(message.strings[1]);
      if(isNaN(message.value)) message.value = message.strings[1];

      let variable = allVariables.filter(function(variable){
        return variable.name === message.variableName;
      })[0];

      if(message.variableName === "version" && haveVersion === false){
        determineSerialType(message.value);
      }
      if(message.variableName === "numSelectors"){
        buildSelectorPicker(message.value);
        toggleVersionedElements(document.getElementById("command-version").getElementsByTagName('span')[0].innerText);
      }

      if(variable != null){
        message.variableInfo = allVariables.filter(function(variable){
          return variable.name === message.variableName;
        })[0];
        if(message.variableInfo.element.tagName === 'BUTTON'){
          updateDualStateButton(message.variableInfo.element, message.value);
        }
        if(message.variableInfo.type === 'slide-box')
          updateSlideBoxValue(message.variableName, message.value);
        else if(message.variableInfo.type === 'advanced')
          updateAdvancedValue(message.variableName, message.value);
        else if(message.variableInfo.type.includes('controller-state'))
          updateControllerStateValue(message.variableName, message.value);
      } else {
        console.log("Message: ", message.variableName, message.value);
      }

      // console.log(message.variableName + ": " + message.value);


    });
  }
};

function onOpen(connectionInfo) {
  if(!chrome.serial.onReceive.hasListeners())
    chrome.serial.onReceive.addListener(recieveData);
  console.log('Connection Info:', connectionInfo);
  if(connectionInfo){
    connectionId = connectionInfo.connectionId;
    if (connectionId == -1) {
      setStatus('Could not open');
      return;
    }
    setStatus('Connected');

    // Get the Serial Version
    console.log("Getting FW Version")
    sendMessage("version");


    connectionManager();
  }
};


function connectionManager(){
  let killConnectionManager = false;
  chrome.serial.getInfo(connectionId, function(ConnectionInfo){
    if(ConnectionInfo.paused) {
      isConnected = false;
      haveVersion = false;
      setStatus("Disconnected");
      beginSerial();
      timeouts.forEach(function(timeout){
        clearTimeout(timeout);
      });
      killConnectionManager = true;
    }
    if(!killConnectionManager){
      setTimeout(function(){
        connectionManager();
      }, 2000);
    }
  });
}

function setStatus(status) {
  document.getElementById('status').innerText = status;
  serialStatus = status;
}

function buildDevicePicker(devices) {
  var eligibleDevices = devices.filter(function(device) {
    return device.productId === 49158 && device.vendorId === 11012;
  });
  if(eligibleDevices.length === 0){
    console.log('no eligibleDevices');
    setTimeout(function(){
      beginSerial();
    },5000);
  }
  console.log("Eligible Devices: ", eligibleDevices);
  var devicePicker = document.getElementById('device-picker');
  devicePicker.innerHTML = "";
  if(eligibleDevices.length > 0){
    eligibleDevices.forEach(function(device) {
      var deviceOption = document.createElement('option');
      let deviceName = device.displayName + ' (' + device.path + ')';
      deviceOption.value = deviceOption.innerText = deviceName;
      deviceOption.id = device.path;
      devicePicker.appendChild(deviceOption);
    });
    openSelectedPort();
  }


  devicePicker.onchange = function() {
    if (connectionId != -1) {
      chrome.serial.close(connectionId, openSelectedPort);
      return;
    }
    openSelectedPort();
  };
}

function openSelectedPort() {
  var devicePicker = document.getElementById('device-picker');
  var selectedPort = devicePicker.options[devicePicker.selectedIndex].id;
  console.log("Selected Port: ",selectedPort);
  chrome.serial.connect(selectedPort, {bitrate: 57600}, onOpen);
}

function beginSerial() {
  chrome.serial.getConnections(function(ConnectionInfo){
    if (connectionId != -1) {
      chrome.serial.disconnect(connectionId, function(){
        console.log('disconnected!');
        chrome.serial.getDevices(function(ports) {
          console.log("Ports: ",ports)
          buildDevicePicker(ports)
        });
      });
    }else {
      chrome.serial.getDevices(function(ports) {
        console.log("Ports: ",ports)
        buildDevicePicker(ports)
      });
    }
  });
};
