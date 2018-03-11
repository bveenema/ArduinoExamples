let isAdvanced = false;
let isConnected = false;

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

  // Initialize Connection manager
  connectionManager();
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

function connectionManager(){
  if(isConnected) { // Enable all inputs
    let inputs = document.getElementsByTagName('input');
    for(let i=0; i<inputs.length; i++){
      inputs[i].disabled = false;
    }
  } else {  // Disable all inputs
    let inputs = document.getElementsByTagName('input');
    for(let i=0; i<inputs.length; i++){
      inputs[i].disabled = true;
    }
  }
  setTimeout(function(){connectionManager()}, 500);
}
