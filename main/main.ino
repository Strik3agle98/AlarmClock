#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <string>
#include <ArduinoJson.h>
const int key1 = 14;
const int key2 = 5;
const int key3 = 4;
const int key4 = 12;
const int key5 = 13;

String url = "http://192.168.43.165:5000/?key=";
WiFiServer server(80);     //เปิดใช้งาน TCP Port 80
DynamicJsonBuffer jsonBuffer;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(key1, INPUT);
  pinMode(key2, INPUT);
  pinMode(key3, INPUT);
  pinMode(key4, INPUT);
  pinMode(key5, INPUT);
  WiFi.begin("Not your WiFi", "Azt3lasdf");
  while (WiFi.status() != WL_CONNECTED)
    {
            delay(500);
            Serial.print(".");
    }
   
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");         
    Serial.println(WiFi.localIP()); 

}

void loop() {
  // put your main code here, to run repeatedly:
  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
   
     HTTPClient http;    //Declare object of class HTTPClient

     if(digitalRead(key1)==HIGH){ 
      http.begin(url+"0");
      http.GET();
      while(digitalRead(key1)==HIGH){
        if(digitalRead(key1)==LOW) break;
      }
     } 
     else if(digitalRead(key2)==HIGH){
      http.begin(url+"12");
      http.GET(); 
      while(digitalRead(key2)==HIGH){
        if(digitalRead(key2)==LOW) break;
      }
     }
     else if(digitalRead(key3)==HIGH){
      http.begin(url+"24"); 
      http.GET(); 
      while(digitalRead(key3)==HIGH){
        if(digitalRead(key3)==LOW) break;
      }
     }
     else if(digitalRead(key4)==HIGH){ 
      http.begin(url+"36");
      http.GET();  
      while(digitalRead(key4)==HIGH){
        if(digitalRead(key4)==LOW) break;
      }
     }
     else if(digitalRead(key5)==HIGH){
      http.begin(url+"48"); 
      http.GET(); 
      while(digitalRead(key5)==HIGH){
        if(digitalRead(key5)==LOW) break;
      }
     }
   
     http.end();  //Close connection
   
  }else{
   
     Serial.println("Error in WiFi connection");   
   
  }
}
