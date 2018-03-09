let allVariables = [];
let getState;

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
  //format variable and value for controller
  let message = variable + ':' + value;

  console.log("Updating variable: " + variable + " to " + value);
  sendMessage(message);
}

// Initial values are values to be retrieved from the beginning but don't change often, such as settings
function getInitialValues() {
  let variables = ["flowRate", "ratioA", "ratioB", "autoReverse", "firmwareID", "version", "name"];

  if(this.index === undefined){
    this.index = 0;
  }else{
    ++this.index;
  }
  if(this.index > variables.length-1){
    this.index = 0;
  }

  sendMessage(variables[this.index]);
  if(this.index < variables.length-1) setTimeout(function(){getInitialValues()},10);
  else {
    console.log('no mas');
    getState = setTimeout(function(){getStateValues()},1000);
  }
}

// State values are values that may or may not be relevant at any given time but change frequently, such as motor speed
function getStateValues(){
  let variables = ["cloudStatus", "motorSpeedA", "motorSpeedB", "action"];

  if(this.index === undefined){
    this.index = 0;
  }else{
    ++this.index;
  }
  if(this.index > variables.length-1){
    this.index = 0;
  }

  sendMessage(variables[this.index]);
  if(this.index < variables.length-1) setTimeout(function(){getStateValues()},10);
  else getState = setTimeout(function(){getStateValues()},1000);
}

// Get a list of all the variables (name attribute) in main.html
function getAllVariables(){
  allVariables =  Array.from(document.querySelectorAll('[name]'))
                .map(function(element){
                  return {type: element.className,  name: element.attributes.name.value};
              });
  console.log(allVariables);
}
