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
      updateController(this.parentNode.getAttribute('name'), parseFloat(this.value, 10));
    }
    textboxElement.onchange = function(){
      updateSibling(sliderElement, parseInt(this.value, 10));
      updateController(this.parentNode.getAttribute('name'), parseFloat(this.value, 10));
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

function updateController(variable, value){
  console.log("Updating variable: " + variable + " to " + value);

  //format variable and value as machine readable message
  let message = variable + ':' + value;

  sendMessage(message);



  
  setTimeout(function(){mock_onRead(variable, value)}, 1000);
}

function mock_onRead(variable, value){
  let elements = document.getElementsByClassName('slide-box');
  for(let i=0; i<elements.length; i++){
    let name = elements[i].getAttribute('name');
    if(name === variable){
      let element = elements[i].getElementsByTagName('span');
      element[0].innerHTML = value.toString(10);
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
