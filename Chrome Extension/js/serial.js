var connectionId = -1;
var readBuffer = "";

function sendMessage(message) {
  let encoder = new TextEncoder();
  let buffer = encoder.encode(message + '\n'); // add newline and encode message for transmission
  chrome.serial.send(connectionId, buffer, function(sendInfo){});
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

      message.variableType = allVariables.filter(function(variable){
        return variable.name === message.variableName;
      })[0].type;

      //console.log(message.variableName + ": " + message.value);

      if(message.variableType === 'slide-box')
        updateSlideBoxValue(message.variableName, message.value);
      else if(message.variableType === 'advanced')
        updateAdvancedValue(message.variableName, message.value);
      else if(message.variableType.includes('controller-state'))
        updateControllerStateValue(message.variableName, message.value);
    });
  }
};

function onOpen(connectionInfo) {
  if(!chrome.serial.onReceive.hasListeners())
    chrome.serial.onReceive.addListener(recieveData);
  console.log('Connection Info:');
  console.log(connectionInfo);
  connectionId = connectionInfo.connectionId;
  if (connectionId == -1) {
    setStatus('Could not open');
    return;
  }
  setStatus('Connected');

  getInitialValues();
};

function setStatus(status) {
  document.getElementById('status').innerText = status;
  if(status === 'Connected') isConnected = 1;
}

function buildDevicePicker(devices) {
  var eligibleDevices = devices.filter(function(device) {
    return device.productId === 49158 && device.vendorId === 11012;
  });
  console.log("Eligible Devices: ");
  console.log(eligibleDevices);
  var devicePicker = document.getElementById('device-picker');
  devicePicker.innerHTML = "";
  eligibleDevices.forEach(function(device) {
    var deviceOption = document.createElement('option');
    let deviceName = device.displayName + ' (' + device.path + ')';
    deviceOption.value = deviceOption.innerText = deviceName;
    deviceOption.id = device.path;
    devicePicker.appendChild(deviceOption);
  });

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
  chrome.serial.connect(selectedPort, {bitrate: 57600}, onOpen);
}

function beginSerial() {
  chrome.serial.getConnections(function(ConnectionInfo){
    if (connectionId != -1) {
      chrome.serial.disconnect(connectionId, function(){
        console.log('disconnected!');
        chrome.serial.getDevices(function(ports) {
          buildDevicePicker(ports)
          openSelectedPort();
        });
      });
    }else {
      chrome.serial.getDevices(function(ports) {
        buildDevicePicker(ports)
        openSelectedPort();
      });
    }
  });
};
