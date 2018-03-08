var connectionId = -1;
var readBuffer = "";

function sendMessage(message) {
  let encoder = new TextEncoder();
  let buffer = encoder.encode(message);
  chrome.serial.send(connectionId, buffer, function(sendInfo){
    console.log('message sent:');
    console.log(sendInfo);
  });
};

function recieveData(readInfo) {
  var uint8View = new Uint8Array(readInfo.data);
  var value = String.fromCharCode(uint8View[0]);

  if (value == "a") // Light on and off
  {
      console.log("CMD[a]: " + readBuffer);
      var opat = isNaN(parseInt(readBuffer)) ? 0 : parseInt(readBuffer);

      document.getElementById('image').style.opacity = (opat* 0.7) + 0.3;
      readBuffer = "";
  }
  else if (value == "b") // Return blink length value
  {
      readBuffer = "";
  }
  else if (value == "c") // Blink Count
  {
      console.log("CMD[c]: " + readBuffer);
      document.getElementById('blinkCount').innerText = readBuffer;
      readBuffer = "";
  }
  else
  {
    readBuffer += value;
  }
};

function onOpen(connectionInfo) {
  console.log('Connection Info:');
  console.log(connectionInfo);
  connectionId = connectionInfo.connectionId;
  if (connectionId == -1) {
    setStatus('Could not open');
    return;
  }
  setStatus('Connected');

  chrome.serial.onReceive.addListener(recieveData);
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
  chrome.serial.connect(selectedPort, onOpen);
}

function beginSerial() {
  if (connectionId != -1) {
    chrome.serial.disconnect(connectionId, function(){
      console.log('disconnected!');
    });
  }

  chrome.serial.getDevices(function(ports) {
    buildDevicePicker(ports)
    openSelectedPort();
  });


};
