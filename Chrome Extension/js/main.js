let allVariables = [];
let timeouts = [];
let haveVersion = false;
let serialType = 0; // 0: {variableName}:{value} , 1: {variableName}:{selector}:{value}

let frequentInterval = 250;
let infrequentInterval = 5000;

onload = function(){
  getAllVariables();
  uiOnLoad();
  beginSerial();
}

function updateMicro(variable, value, stepSize){
    //if value is float, multiply by 100 and truncate
  if(stepSize < 1){
    value = Math.round(value*100);
  }
  console.log("Updating variable: " + variable + " to " + value);
  sendMessage(formatMessage(variable, value));
}

function determineSerialType(version){
  console.log("FW Version is: " + version);
  if(version > 4 && version < 1000){
    serialType = 1;
    sendMessage(formatMessage("numSelectors"));
  } else {
    serialType = 0;
  }
  haveVersion = true;
}

function formatMessage(variableName, newValue = "null", selector = -1){
  let message = variableName;
  if(serialType === 1){
    if(selector === -1){
      let selectorPicker = document.getElementById("selector-picker");
      message += ":" + selectorPicker.value;
    } else {
      message += ":" + selector;
    }
  }
  if(newValue !== "null"){
    message += ":" + newValue;
  }
  return message;
}

// Initial values are values to be retrieved from the beginning but don't change often, such as settings
function getInitialValues() {
  let variables = allVariables.map(function(variable){
    return variable.name;
  })//["flowRate", "ratioA", "ratioB", "autoReverse", "firmwareID", "version", "name", "stepsPerMlA", "stepsPerMlB"];

  if(this.initialIndex === undefined){
    this.initialIndex = 0;
  }else{
    ++this.initialIndex;
  }
  if(this.initialIndex > variables.length-1){
    this.initialIndex = 0;
  }

  // console.log("Current Variable: ", variables[this.initialIndex]);

  sendMessage(formatMessage(variables[this.initialIndex]));
  if(this.initialIndex < variables.length-1) timeouts.push(setTimeout(function(){getInitialValues()},10));
  else {
    timeouts.forEach(function(timeout){
      clearTimeout(timeout);
    });
    timeouts.push(setInterval(function(){getFrequentStateValues()},frequentInterval));
    timeouts.push(setInterval(function(){getInfrequentStateValues()},infrequentInterval));
  }
}

// Frequent State values are values that may or may not be relevant at any given time but change frequently, such as motor speed
function getFrequentStateValues(){
  let variables = allVariables.filter(function(variable){
    return variable.type === "controller-state-frequent"
  }).map(function(variable){
    return variable.name;
  });

  if(this.frequentIndex === undefined){
    this.frequentIndex = 0;
  }else{
    ++this.frequentIndex;
  }
  if(this.frequentIndex > variables.length-1){
    this.frequentIndex = 0;
  }

  //console.log('frequent: ' + variables[this.frequentIndex]);
  sendMessage(formatMessage(variables[this.frequentIndex]));
  if(this.frequentIndex < variables.length-1) timeouts.push(setTimeout(function(){getFrequentStateValues()},10));
}

// Infrequent State values are values that may or may not be relevant at any given time but change occasionally, such as cloud status
function getInfrequentStateValues(){
  timeouts.forEach(function(timeout){
    clearTimeout(timeout);
  });
  let variables = allVariables.filter(function(variable){
    return variable.type === "controller-state-infrequent"
  }).map(function(variable){
    return variable.name;
  });

  if(this.infrequentIndex === undefined){
    this.infrequentIndex = 0;
  }else{
    ++this.infrequentIndex;
  }
  if(this.infrequentIndex > variables.length-1){
    this.infrequentIndex = 0;
  }

  //console.log('infrequent: ' + variables[this.infrequentIndex]);
  sendMessage(formatMessage(variables[this.infrequentIndex]));
  if(this.infrequentIndex < variables.length-1) timeouts.push(setTimeout(function(){getInfrequentStateValues()},10));
  else {
    timeouts.push(setInterval(function(){getFrequentStateValues()},frequentInterval));
    timeouts.push(setInterval(function(){getInfrequentStateValues()},infrequentInterval));
  }
}

// Get a list of all the variables (name attribute) in main.html
function getAllVariables(){
  allVariables =  Array.from(document.querySelectorAll('[id^=command]'))
                .filter(function(variable){
                  return !variable.className.includes("hidden")
                })
                .map(function(element){
                  let type = element.className.split(" ").filter(function(el){
                    return  el === "slide-box" ||
                            el === "advanced" ||
                            el === "controller-state-infrequent" ||
                            el === "controller-state-frequent" ||
                            el === "controller-state-once"
                  })[0];
                  return {
                    type: type,
                    name: element.id.split("-")[1],
                    element: element,
                    defaultValue: element.getElementsByTagName('span')[0].innerText
                  };
                });
  // Add Dual State Buttons
  // if button is dual state - add it to the controller-state-frequent list
  let dualStateButtons = document.getElementsByClassName('dual-state-button')
  for(let i=0; i<dualStateButtons.length; i++){
    let button = dualStateButtons[i];
    allVariables.push({
      type: 'dual-state-button',
      name: button.getAttribute('id'),
      element: button,
      defaultValue: button.getAttribute('value')
    });
    console.log("Added Button: ",allVariables);
  }
  console.log(allVariables);
}
