onload = function(){
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

function getInitialValues() {
  let variables = ["flowRate", "ratioA", "ratioB", "autoReverse"];

  if(this.index === undefined){
    this.index = 0;
  }else{
    ++this.index;
  }

  sendMessage(variables[this.index]);

  if(this.index < variables.length-1) setTimeout(function(){getInitialValues()},100);
}
