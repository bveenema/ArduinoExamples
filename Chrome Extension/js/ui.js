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
      let commandName = this.parentNode.getAttribute('id').split('-')[1];
      updateMicro(commandName, parseFloat(this.value, 10), this.getAttribute('step'));
    }
    textboxElement.onchange = function(){
      updateSibling(sliderElement, parseInt(this.value, 10));
      let commandName = this.parentNode.getAttribute('id').split('-')[1];
      updateMicro(commandName, parseFloat(this.value, 10), this.getAttribute('step'));
    }
  }

  // Attach listeners to Advanced settings
  let advancedSettings = document.getElementsByClassName('advanced');
  for(let i=0; i<advancedSettings.length; i++){
    let advancedElement = advancedSettings[i].getElementsByClassName('textbox')[0];

    advancedElement.onchange = function(){
      let commandName = this.parentNode.getAttribute('id').split('-')[1];
      updateMicro(commandName, parseFloat(this.value, 10), this.getAttribute('step'));
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

  // Attach Listeners to Buttons (if not hidden)
  let buttons = document.getElementsByClassName('action-button');
  for(let i=0; i<buttons.length; i++){
    let button = buttons[i]
    console.log(button);
    button.onclick = function(){
      console.log("toggle", button.innerText.split(" ")[1]);
      sendMessage(formatMessage(button.getAttribute('id'),1));
      if(button.nextElementSibling && button.nextElementSibling.classList.contains('button-alt-text')){
        let swapText = button.innerText;
        button.innerText = button.nextSibling.innerText;
        button.nextSibling.innerText = swapText;
        button.classList.toggle('enabled');
      }
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
    let name = elements[i].getAttribute('id').split("-")[1];
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
    let name = elements[i].getAttribute('id').split("-")[1];
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
    let name = elements[i].getAttribute('id').split("-")[1];
    if(name === variable){
      let current = elements[i].getElementsByTagName('span')[0];
      current.innerHTML = value.toString(10);
      if(variable === "error"){
        if(value === "none" || value === ""){
          document.getElementById('command-error').style = "display: none";
        }else{
          document.getElementById('command-error').style = "display: block";
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
    let inputs = Array.from(document.getElementsByTagName('input'));
    let buttons = Array.from(document.getElementsByClassName('action-button'));
    let elements = inputs.concat(buttons);
    for(let i=0; i<elements.length; i++){
      elements[i].disabled = false;
    }
  } else {
    // Disable all inputs and buttons, set all variables to default
    allVariables.forEach(function(variable){
      let inputs = Array.from(variable.element.getElementsByTagName('input'));
      let buttons = Array.from(document.getElementsByClassName('action-button'));
      let elements = inputs.concat(buttons);
      for(let i=0; i<elements.length; i++){
        elements[i].disabled = true;
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
  var potentials = document.querySelectorAll("[class*='version:']");

  // convert potentials (nodelist) to array
  var potentialsArray = [];
  for(var i = potentials.length; i--; potentialsArray.unshift(potentials[i]));

  // make sure element has class name starting with "showMinVersion-", add version number to element
  let elements = potentialsArray.filter(function(element){
    let returnVal = false;
    for(i=0;i<element.classList.length; i++){
      if(element.classList[i].match(/^version:/)){
        element.version = element.classList[i].split(':')[1];
        returnVal = true;
        break;
      }
    }
    return returnVal;
  });

  // display element if element.version is <= version
  elements.forEach(function(element){
    let selectorVersion = element.version.split('-')[1];
    if(element.version.startsWith("all")){
      document.getElementById(element.id).classList.remove('hidden');
    }
    if(element.version.startsWith("min")){
      if((selectorVersion <= version) && version < 1000){
        document.getElementById(element.id).classList.remove('hidden');
      }
    } else if(element.version.startsWith("max")){
      if((selectorVersion <= version) && version < 1000){
        document.getElementById(element.id).classList.add('hidden');
      }
    }
    // if((element.version <= version) && (version < 1000)){
    //   if(show === true){
    //     document.getElementById(element.id).removeAttribute('hidden');
    //   } else {
    //     document.getElementById(element.id).setAttribute('hidden', true);
    //   }
    // }
  })
  getAllVariables();
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
