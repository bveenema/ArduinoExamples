let allVariables = [];
let timeouts = [];

let frequentInterval = 100;
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
  //format variable and value for controller
  let message = variable + ':' + value;

  console.log("Updating variable: " + variable + " to " + value);
  sendMessage(message);
}

// Initial values are values to be retrieved from the beginning but don't change often, such as settings
function getInitialValues() {
  let variables = allVariables.map(function(variable){
    return variable.name;
  })//["flowRate", "ratioA", "ratioB", "autoReverse", "firmwareID", "version", "name", "stepsPerMlA", "stepsPerMlB"];

  if(this.index === undefined){
    this.index = 0;
  }else{
    ++this.index;
  }
  if(this.index > variables.length-1){
    this.index = 0;
  }

  //console.log('initial: ' + variables[this.index]);
  sendMessage(variables[this.index]);
  if(this.index < variables.length-1) timeouts.push(setTimeout(function(){getInitialValues()},10));
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
  sendMessage(variables[this.frequentIndex]);
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

  if(this.index === undefined){
    this.index = 0;
  }else{
    ++this.index;
  }
  if(this.index > variables.length-1){
    this.index = 0;
  }

  //console.log('infrequent: ' + variables[this.index]);
  sendMessage(variables[this.index]);
  if(this.index < variables.length-1) timeouts.push(setTimeout(function(){getInfrequentStateValues()},10));
  else {
    timeouts.push(setInterval(function(){getFrequentStateValues()},frequentInterval));
    timeouts.push(setInterval(function(){getInfrequentStateValues()},infrequentInterval));
  }
}

// Get a list of all the variables (name attribute) in main.html
function getAllVariables(){
  allVariables =  Array.from(document.querySelectorAll('[name]'))
                .map(function(element){
                  return {type: element.className,  name: element.attributes.name.value};
              });
  console.log(allVariables);
}
