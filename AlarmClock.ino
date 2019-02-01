// Demo of MAX7219_Dot_Matrix library - sideways scrolling
// Author: Md. Khairul Alam
// Date: 5 July 2018

#include <SPI.h>
//#include <bitBangedSPI.h>
//#include <MAX7219_Dot_Matrix.h>
#include <ESP8266HTTPClient.h>
#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
const byte chips = 8; //number of matrix in the display

// for NodeMCU 1.0
#define DIN_PIN 15  // D8
#define CS_PIN  13  // D7
#define CLK_PIN 12  // D6

//MAX7219_Dot_Matrix display (chips, CS_PIN, DIN_PIN, CLK_PIN);  // 2 chips, then specify the LOAD, DIN, CLK pins

WiFiClient client;

String weatherMain = "";
String weatherDescription = "";
String weatherLocation = "";
String country;
int humidity;
int pressure;
float temp;
float tempMin, tempMax;
int clouds;
float windSpeed;
String date;
int h,m,s;
int sunrise, sunset;
String payload;
String weatherString;

// =======================================================================
// CHANGE YOUR CONFIG HERE:
// =======================================================================
int timezone = 7; //GMT+7
const char* ssid     = "arunee784 2G";     // SSID of local network
const char* password = "cankim01";   // Password on network
String weatherKey = "280583189452d44eebf09fe2ad36b3f2"; //Strik3agle's API, get your own API key
String weatherLang = "&lang=en";
String cityID = "1609350"; //Bangkok
// read OpenWeather api description for more info
// =======================================================================

char message1 [] = "This is a testing display. Designed by Md. Khairul Alam";
char weather_message[300];
char time_message[50];
char date_message[50];

void setup ()
  {
//  display.begin ();
//  display.setIntensity (1);
  lcd.begin(20, 4);
  lcd.setCursor(0,1);
  Serial.begin(115200);
  Serial.print("Connecting WiFi ");
  lcd.print("Connecting");
  WiFi.begin(ssid, password);
  //printStringWithShift("Connecting",15);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lcd.print(".");
  }
  Serial.println("");
  Serial.print("Connected: "); Serial.println(WiFi.localIP());
  lcd.home();
  lcd.setCursor(5,1);
  lcd.clear();
  //getWeatherData();
  String str = weatherString;
  int str_len = str.length() + 1; 
  str.toCharArray(weather_message, str_len) ;   

  getTime();
  
}  // end of setup

unsigned long lastMoved = 0;
unsigned long MOVE_INTERVAL = 2;  // mS
int  messageOffset;

//void updateDisplay (char *msg)
//  {
//  //String message = msg;
//  display.sendSmooth (msg, messageOffset);
//  
//  // next time show one pixel onwards
//  if (messageOffset++ >= (int) (strlen (msg) * 8))
//    messageOffset = - chips * 8;
//  }  // end of updateDisplay
  
long last = millis();
long lastTime = millis();

void loop () 
  { 
  if((millis()- last)>1000*60*15){  //update weather every 15 minutes 
    //getWeatherData();
    getTime();
    //String str = weatherString;
    //int str_len = str.length() + 1;
    //str.toCharArray(weather_message, str_len) ;
    last = millis();
   }
   
//  // update display if time is up
//  if(s>0 && s<40){
//  if (millis () - lastMoved >= MOVE_INTERVAL)
//    {
//      //updateDisplay(weather_message);
//      lastMoved = millis ();
//    }  
//  }
//
//  if(s>40 && s<55){
//  if (millis () - lastMoved >= MOVE_INTERVAL)
//    {
//      //updateDisplay(date_message);
//      lastMoved = millis ();
//    }  
//  }
//  /*
//  // do other stuff here    
//  if(millis()-lastTime>65000){ 
//    long now = millis();
//    do{
//      updateTime();
//      showTime();
//      }while(millis()-now<40000);
//    now = millis();
//    lastTime = millis();
//  }  
//   */
 showTime(); 
   
 updateTime();

 if(millis() < last) last = 0;
}  // end of loop


// =======================================================================
// retrive weather data

//const char *weatherHost = "api.openweathermap.org";

void getWeatherData()
{
//  Serial.print("connecting to "); Serial.println(weatherHost);
//  if (client.connect(weatherHost, 80)) {
//    client.println(String("GET /data/2.5/weather?id=") + cityID + "&units=metric&appid=" + weatherKey + weatherLang + "\r\n" +
//                "Host: " + weatherHost + "\r\nUser-Agent: ArduinoWiFi/1.1\r\n" +
//                "Connection: close\r\n\r\n");
//  } else {
//    Serial.println("connection failed");
//    return;
//  }
//  String line;
//  int repeatCounter = 0;
//  while (!client.available() && repeatCounter < 10) {
//    delay(500);
//    Serial.println("w.");
//    repeatCounter++;
//  }
//  while (client.connected() && client.available()) {
//    char c = client.read(); 
//    if (c == '[' || c == ']') c = ' ';
//    line += c;
//  }
//  Serial.println(line);
//  client.stop();

  HTTPClient http;
  http.begin("https://api.openweathermap.org/data/2.5/weather?id=1609350&units=metric&appid=280583189452d44eebf09fe2ad36b3f2");
  int httpCode = http.GET(); //still doesn't work
  Serial.print(httpCode);
  Serial.print("Connecting to api.openweathermap.org");
  while(httpCode <= 0){
    Serial.print(".");
    delay(500);
  }
  payload=http.getString();
  Serial.println(payload);
  http.end();
  //get data from api
  
  DynamicJsonBuffer jsonBuf;
  JsonObject &root = jsonBuf.parseObject(payload);
  if (!root.success())
  {
    Serial.println("parseObject() failed");
    return;
  }
  //weatherMain = root["weather"]["main"].as<String>();
  weatherDescription = root["weather"]["description"].as<String>();
  weatherDescription.toLowerCase();
  Serial.println(weatherDescription);
  //  weatherLocation = root["name"].as<String>();
  //  country = root["sys"]["country"].as<String>();
  sunrise = root["sys"]["sunrise"];
  sunset = root["sys"]["sunset"];
  temp = root["main"]["temp"];
  humidity = root["main"]["humidity"];
  pressure = root["main"]["pressure"];
  tempMin = root["main"]["temp_min"];
  tempMax = root["main"]["temp_max"];
  windSpeed = root["wind"]["speed"];
  clouds = root["clouds"]["all"];
  //String deg = String(char('~')+131);
  //weatherString = "  Temp: " + String(temp,1) /*+ deg*/ + "C (" + String(tempMin,1) /*+ deg*/ + "-" + String(tempMax,1) /*+ deg*/ + ")  ";
  weatherString = "  Temp: " + String(temp,1) /*+ deg*/ + "C";
  //weatherString += weatherDescription;
  weatherString += "  Humidity: " + String(humidity) + "% ";
  weatherString += "  Pressure: " + String(pressure) + "hPa ";
  weatherString += "  Clouds: " + String(clouds) + "%  ";
  weatherString += "  Wind: " + String(windSpeed,1) + "m/s   ";
  Serial.println(weatherString);
}


// =======================================================================

float utcOffset = 2;
long localEpoc = 0;
long localMillisAtUpdate = 0;

void getTime()
{
  WiFiClient client;
  if (!client.connect("www.google.com", 80)) {
    Serial.println("connection to google failed");
    return;
  }
  Serial.println("Connected to google");
  client.print(String("GET / HTTP/1.1\r\n") +
               String("Host: www.google.com\r\n") +
               String("Connection: close\r\n\r\n"));
  int repeatCounter = 0;
  while (!client.available() && repeatCounter < 10) {
    delay(500);
    //Serial.println(".");
    repeatCounter++;
  }

  String line;
  client.setNoDelay(false);
  while(client.connected() && client.available()) {
    line = client.readStringUntil('\n');
    line.toUpperCase();
    if (line.startsWith("DATE: ")) {
      date = ""+line.substring(6, 22);
      h = line.substring(23, 25).toInt()+timezone; 
      m = line.substring(26, 28).toInt();
      s = line.substring(29, 31).toInt();
      localMillisAtUpdate = millis();
      localEpoc = (h * 60 * 60 + m * 60 + s);

      Serial.println(line);
      String clock_date = date;
      int date_len = clock_date.length() + 1; 
      clock_date.toCharArray(date_message, date_len) ;
    
    }
  }
  client.stop();
}

// =======================================================================

void updateTime()
{
  long curEpoch = localEpoc + ((millis() - localMillisAtUpdate) / 1000);
  long epoch = round(curEpoch + 3600 * utcOffset + 86400L) % 86400L;
  h = ((epoch  % 86400L) / 3600) % 24;
  m = (epoch % 3600) / 60;
  s = epoch % 60;
}

// =======================================================================
long lastmillis=millis();
void showTime(){
  lcd.setCursor(6,1);
  String timeString = "";
  timeString+= h/10 ? h/10 : 0;
  timeString+= h%10;
  timeString+= ":";
  timeString+= m/10;
  timeString+= m%10;
  timeString+= ":";
  timeString+= s/10;
  timeString+= s%10;   
  String clock_time = timeString;
  int clock_len = clock_time.length() + 1; 
  clock_time.toCharArray(time_message, clock_len) ;
  lcd.print(time_message);
}
