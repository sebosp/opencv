//Pins
int temperaturePin=0;
int lightSensingPin = 1;
int highTempPin = 12;

//Variables
int delayCounter=0;
int incomingMsg = 0;
float temp = 0.0;
int lightSensed = 0;
int softPot = 0;

void setup(){
  Serial.begin(9600);
  delay(1000);
}

float getTemperatureFromTMP36(){
  return ((analogRead(temperaturePin) * 0.004882814) - 0.5) * 100;
}

int getLightLevel(){
  return constrain(map(analogRead(lightSensingPin),0,900,255,0),0,255);
}

int getSoftPot(){
  return constrain(map(analogRead(2),0,1024,0,255),0,255);
}
void loop(){
  if (delayCounter > 1000){
    temp=getTemperatureFromTMP36();
    lightSensed=getLightLevel();
    softPot = getSoftPot();
    Serial.print("Temp: ");
    Serial.print(temp);
    Serial.print(". Light: ");
    Serial.print(lightSensed);
    Serial.print(". softPot: ");
    Serial.println(softPot);
    delayCounter = 0;
  }
  if (Serial.available() > 0){
    incomingMsg = Serial.read();
    Serial.print("I received: ");
    Serial.println(incomingMsg,DEC);
  }
  delayCounter++;
  delay(1);
}
