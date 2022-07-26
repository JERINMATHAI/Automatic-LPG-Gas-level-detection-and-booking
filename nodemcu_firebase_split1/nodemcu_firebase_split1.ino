#include <Wire.h>
#include <Firebase_ESP_Client.h>
#include <ESP8266WiFi.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define DATA_URL "https://sample-projecct-13f05-default-rtdb.firebaseio.com/"
#define API_KEY "AIzaSyB9BGf91_cJ5qXGJs85YzUZQDSlH7YvsCs"


FirebaseAuth auth;
FirebaseConfig config;
FirebaseData db;
bool signedUp = false;

String WIFI_SSID = "Jerin",
WIFI_PSWD = "Jerin@123";

String data = "0,0";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); 
  Wire.begin(4,5);
  WiFi.begin(WIFI_SSID,WIFI_PSWD);
  Serial.println("Connecting to "+String(WIFI_SSID)+"....");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected! IP: ");
  Serial.print(WiFi.localIP());
  Serial.println();

  config.api_key = API_KEY;
  config.database_url = DATA_URL;
  if(Firebase.signUp(&config,&auth,"","")){
    Serial.println("Logged into Firebase");
    signedUp = true;
  }
  else
    Serial.printf("%s\n",config.signer.signupError.message.c_str());
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config,&auth);
  Firebase.reconnectWiFi(true);

}
float weight=0.0;
int gSen=0;
void loop() {
  // put your main code here, to run repeatedly:
  data = "";
  Wire.requestFrom(9,15);
  while(Wire.available()){
    byte in = Wire.read();
    if(in < 34 || in > 126)
      break;
    data += (char)in;
  }
  Serial.println(data);
  int index = data.indexOf(','),
  len = data.length();
  weight = data.substring(0,index).toFloat();
  gSen = data.substring(index+1,len).toInt();
  
  if(Firebase.ready() && signedUp){
    if(Firebase.RTDB.setFloat(&db,"WeightSensor/float",weight)){
      Serial.println("Weight updated!");
//      Serial.println("Path :" + db.dataPath());
//      Serial.println("Type :" + db.dataType());
    }
    if(Firebase.RTDB.setInt(&db,"GasSensor/int",gSen)){
      Serial.println("Gas Sensor updated!");
//      Serial.println("Path :" + db.dataPath());
//      Serial.println("Type :" + db.dataType());
    }
  }
  else{
    Serial.println("Oops! Something went wrong");
    Serial.println("Error: " + db.errorReason());
  }
  delay(100);
}  
