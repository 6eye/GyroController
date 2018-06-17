#include <ESP8266WiFi.h>
//Single client server

//Store all setting in varaiables for easier modification
const char* ssid     = "";      // SSID | WiFi network name
const char* password = "";      // Network Password

const int   port = 8080;           
WiFiServer server(8080);  //Set server port for 8080 | 1024-49151 

void setup() {
  //Start comunication at serial lines with 115200 bits/sec
  Serial.begin(115200);
  
  WiFi.begin(ssid, password); //Sart and connect to wifi
  //Wait untill conection is establisehd
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    
  }

 
  //Start server
  server.begin();
 

}

void loop() {
  //Wait for client to connect
  WiFiClient client = server.available();
  if(client){
    
    while(client.connected()){
      //While client is conected and || Serial buffer contains data send read data  to client
      if(Serial.available()){
     
      client.println(Serial.read());
      }
    }
  }
  
}
