var connectionId = -1;
var readBuffer = "";

function setPosition(position) {
  var buffer = new ArrayBuffer(1);
  var uint8View = new Uint8Array(buffer);
  uint8View[0] = position;
  chrome.serial.write(connectionId, buffer, function() {});
};

function onRead(readInfo) {
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
  // Keep on reading.
  chrome.serial.read(connectionId, 1, onRead);
};

function onOpen(openInfo) {
  connectionId = openInfo.connectionId;
  console.log("connectionId: " + connectionId);
  if (connectionId == -1) {
    setStatus('Could not open');
    return;
  }
  setStatus('Connected');

  setPosition(0);
  chrome.serial.read(connectionId, 1, onRead);
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
  var devicePicker = document.getElementById('port-picker');
  var selectedPort = devicePicker.options[devicePicker.selectedIndex].value;
  chrome.serial.open(selectedPort, onOpen);
}

function beginSerial() {
  setTimeout(function(){setStatus('Connected')}, 5000);

  chrome.serial.getDevices(function(ports) {
    buildDevicePicker(ports)
    openSelectedPort();
  });


};
