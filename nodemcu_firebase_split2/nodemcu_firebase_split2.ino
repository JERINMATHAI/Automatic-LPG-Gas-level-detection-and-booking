#include <HX711.h>
#include <Wire.h>

#define DOUT 9
#define CLK 10

#define buzzer 5

#define gSensor A0

HX711 scale;

float calFactor = 442;

String data = "0,0";
void requestEvent(){
  Wire.write(data.c_str());
}

void receiveEvent(int hy){
  String nodeData = "";
  while(Wire.available());
    nodeData += (char)Wire.read();
  Serial.println(nodeData+" SFD");
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  scale.begin(DOUT,CLK);
  Serial.println("Calibrate Weight Sensor?\nEnter 1 to enter calibration or 0 to skip this.");
  while(!Serial.available());
  int opt = Serial.parseInt();
  if(opt)
    calibrateLC();
  Serial.println("Remove all weights. Press any key to continue.");
  while(!Serial.available());
  scale.set_scale(calFactor);
  scale.tare();
  pinMode(gSensor,INPUT);
  pinMode(buzzer,OUTPUT);
  Wire.begin(9);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  Serial.println("Updating values on Firebase...");
}

void loop() {
  // put your main code here, to run repeatedly:
  float weight = scale.get_units();
  int gSen = analogRead(gSensor);
  if(gSen > 500)
    digitalWrite(buzzer,HIGH);
  else
    digitalWrite(buzzer,LOW);
  Serial.println("Weight :"+String(weight,4));
  Serial.println("Gas sensor reading: "+String(gSen));
  data = String(weight,4)+","+String(gSen);
  delay(10);
}

void calibrateLC(){
  int currWeight = scale.get_units();
  scale.tare();
  Serial.println("Place the weight and enter its actual weight(in grams).");
  while(!Serial.available());
  int calWeight = Serial.parseInt();
  Serial.println("Starting calibration, do not move the weights");
  delay(5000);
  Serial.print("Calibrating");
  float error = abs((float)(calWeight) - currWeight),prevError = error;
  long prevMillis = 0;
  bool increment = true;
  while(error > 3){
    if(increment)
      calFactor += 1;
    else
      calFactor -= 1;
    scale.set_scale(calFactor);
    currWeight = scale.get_units();
    error = abs((float)(calWeight) - currWeight);
    if(error > prevError)
      increment = !increment;
    prevError = error;
    if(millis() - prevMillis > 3000)
      Serial.print(".");
    delay(10);
  }
  Serial.println();
  Serial.println("Calibration complete!\n You may now remove the weight");
  Serial.println("Calibration factor set at : "+String(calFactor));
  Serial.println("Change the \"calFactor\" variable in the code to the above value to prevent recalibration");
  delay(1000);
}
