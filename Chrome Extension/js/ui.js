let isAdvanced = false;

function uiOnLoad(){

  // Attach listeners to slide boxes
  let slideBoxes = document.getElementsByClassName('slide-box');
  for(let i=0; i<slideBoxes.length; i++){
    let sliderElement = slideBoxes[i].getElementsByClassName('slider')[0];
    let textboxElement = slideBoxes[i].getElementsByClassName('textbox')[0];

    sliderElement.oninput = function(){
      updateSibling(textboxElement, parseFloat(this.value, 10));
    }
    sliderElement.onchange = function(){
      updateMicro(this.parentNode.getAttribute('name'), parseFloat(this.value, 10), this.getAttribute('step'));
    }
    textboxElement.onchange = function(){
      updateSibling(sliderElement, parseInt(this.value, 10));
      updateMicro(this.parentNode.getAttribute('name'), parseFloat(this.value, 10), this.getAttribute('step'));
    }
  }

  // Attach listeners to Advanced settings
  let advancedSettings = document.getElementsByClassName('advanced');
  for(let i=0; i<advancedSettings.length; i++){
    let advancedElement = advancedSettings[i].getElementsByClassName('textbox')[0];

    advancedElement.onchange = function(){
      updateMicro(this.parentNode.getAttribute('name'), parseFloat(this.value, 10), this.getAttribute('step'));
    }
  }

  // Attach listener to Advanced Expander
  let advancedExpander = document.getElementById('advanced-expander');
  advancedExpander.onclick = function(){
    advancedOptionsManager(this);
  }

  // Attach listener to Refresh Devices button
  let refreshButton = document.getElementById('refresh-devices');
  refreshButton.onclick = function(){
    beginSerial();
  }

  // Attach listener to Toggle Motor button
  let toggleMotorButton = document.getElementById('toggle-motor');
  toggleMotorButton.onclick = function(){
    sendMessage("toggleMotor");
  }

  // Attach listener to Wifi Button
  let toggleWifiButton = document.getElementById('toggle-wifi');
  toggleWifiButton.onclick = function(){
    if(toggleWifiButton.innerText === "Enable Wifi"){
      console.log("Enabling Wifi");
      toggleWifiButton.innerText = "Disable Wifi"
      toggleWifiButton.classList.toggle("enabled");
      sendMessage(formatMessage("wifiStatus",1));
    } else {
      console.log("Disabling Wifi");
      toggleWifiButton.innerText = "Enable Wifi"
      toggleWifiButton.classList.toggle("enabled");
      sendMessage(formatMessage("wifiStatus",0));
    }

  }

  // Initialize Connection view
  setConnectionView();
}

function updateSibling(sibling,value){
  sibling.value = value;
}

function updateSlideBoxValue(variable, value){
  let elements = document.getElementsByClassName('slide-box');
  for(let i=0; i<elements.length; i++){
    let name = elements[i].getAttribute('name');
    if(name === variable){
      let stepSize = parseFloat(elements[i].getElementsByClassName('slider')[0].getAttribute('step'));
      if(stepSize < 1) value /= 100;

      let slider = elements[i].getElementsByClassName('slider')[0];
      let textbox = elements[i].getElementsByClassName('textbox')[0];
      let current = elements[i].getElementsByTagName('span')[0];
      slider.value = value;
      textbox.value =value;
      current.innerHTML = value.toString(10);
    }
  }
}

function updateAdvancedValue(variable, value){
  let elements = document.getElementsByClassName('advanced');
  for(let i=0; i<elements.length; i++){
    let name = elements[i].getAttribute('name');
    if(name === variable){
      let textbox = elements[i].getElementsByClassName('textbox')[0];
      let current = elements[i].getElementsByTagName('span')[0];
      textbox.value =value;
      current.innerHTML = value.toString(10);
    }
  }
}

function updateControllerStateValue(variable, value){
  if(value === "wait") return;
  let elements = document.querySelectorAll('.controller-state-frequent, .controller-state-infrequent, .controller-state-once');
  for(let i=0; i<elements.length; i++){
    let name = elements[i].getAttribute('name');
    if(name === variable){
      let current = elements[i].getElementsByTagName('span')[0];
      current.innerHTML = value.toString(10);
      if(variable === "error"){
        if(value === "none" || value === ""){
          document.getElementById('error').style = "display: none";
        }else{
          document.getElementById('error').style = "display: block";
        }
      }
    }
  }
}

function advancedOptionsManager(advancedExpander){
  //Toggle Advanced Options
  isAdvanced = !isAdvanced;
  if(isAdvanced){
    advancedExpander.getElementsByTagName('img')[0].src = "assets/plus-box.svg";
    //inject advanced template
    document.getElementById('advanced-input').style = "display: inline-block";
  } else {
    advancedExpander.getElementsByTagName('img')[0].src = "assets/plus.svg";
    document.getElementById('advanced-input').style = "display: none";
  }
}

function setConnectionView(){
  if(serialStatus === "Connected") { // Enable all inputs
    let inputs = document.getElementsByTagName('input');
    for(let i=0; i<inputs.length; i++){
      inputs[i].disabled = false;
    }
  } else {
    // Disable all inputs and set all variables to default
    allVariables.forEach(function(variable){
      let inputs = variable.element.getElementsByTagName('input');
      for(let i=0; i<inputs.length; i++){
        inputs[i].disabled = true;
      }
      if(variable.type === 'slide-box') updateSlideBoxValue(variable.name, variable.defaultValue)
      else if(variable.type === 'advanced') updateAdvancedValue(variable.name, variable.defaultValue)
      else if(variable.type === 'controller-state-infrequent' ||
              variable.type === 'controller-state-frequent' ||
              variable.type === 'controller-state-once') {
                updateControllerStateValue(variable.name, variable.defaultValue)
              }
    })

    // hide Versioned Elements
    toggleVersionedElements(999, false);

    // Clear Device Picker

  }
  setTimeout(function(){setConnectionView()}, 500);
}

function toggleVersionedElements(ver, show = true){
  let version = 0;
  if(isNaN(Number(ver))) {
    return;
  }else{
    version = Number(ver);
  }
  var potentials = document.querySelectorAll("[class*=showMinVersion-]");

  // convert potentials (nodelist) to array
  var potentialsArray = [];
  for(var i = potentials.length; i--; potentialsArray.unshift(potentials[i]));

  // make sure element has class name starting with "showMinVersion-", add version number to element
  let elements = potentialsArray.filter(function(element){
    let returnVal = false;
    for(i=0;i<element.classList.length; i++){
      if(element.classList[i].match(/^showMinVersion-/)){
        element.version = Number(element.classList[i].split('-')[1]);
        returnVal = true;
        break;
      }
    }
    return returnVal;
  });

  // display element if element.version is <= version
  elements.forEach(function(element){
    if((element.version <= version) && (version < 1000)){
      if(show === true){
        document.getElementById(element.id).removeAttribute('hidden');
      } else {
        document.getElementById(element.id).setAttribute('hidden', true);
      }
    }
  })
}

function buildSelectorPicker(numSelectors) {
  console.log("Building Selectors: " + numSelectors);

  let selector = document.getElementById('selector-picker');
  selector.innerHTML = "";

  for(i=0;i<numSelectors;i++){
    let selectorOption = document.createElement('option');
    selectorOption.value = i;
    selectorOption.innerText = i;
    selector.appendChild(selectorOption);
  }
  selector.onchange = function() {
    getInitialValues();
  }
  getInitialValues();
}
