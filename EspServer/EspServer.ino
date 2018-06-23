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

       String data_buffer="";
       char   char_buffer;
      //While client is conected and || Serial buffer contains data send read data  to client
      if(Serial.available()){
          char_buffer=Serial.read(); //Read Single Char form Serial
          
          if(char_buffer=='(' ){  //If char is '(' then new data package is coming || Reset data_buffer
            data_buffer="";
          }
          else{
            if(char_buffer==')'){  //If cahr is ')' it is end of data package || So send data to client
              //Maybe add some error detection here in futere
              client.println(data_buffer);
              
            }else{
              data_buffer=data_buffer+char_buffer;
            } 
          }
         
      }
    }
  }
  
}
