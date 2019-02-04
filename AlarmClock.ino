// Demo of MAX7219_Dot_Matrix library - sideways scrolling
// Author: Md. Khairul Alam
// Date: 5 July 2018

#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
const byte chips = 8; //number of matrix in the display
#define DHTPIN            14 //Pin connected to DHT
int relayPin = 15;
int switchPin = 16;
int state = 0;
// Uncomment the type of sensor in use:
//#define DHTTYPE           DHT11     // DHT 11 
#define DHTTYPE           DHT22     // DHT 22 (AM2302)
//#define DHTTYPE           DHT21     // DHT 21 (AM2301)


DHT_Unified dht(DHTPIN, DHTTYPE);


WiFiClient client;

int humidity;
float temp;
String date;
int h,m,s;
int sunrise, sunset;
String payload;
String weatherString;

// =======================================================================
// CHANGE YOUR CONFIG HERE:
// =======================================================================
//int timezone = 7; //GMT+7
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
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
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
  updateTemp();
  getTime();
  
}  // end of setup

unsigned long lastMoved = 0;
unsigned long MOVE_INTERVAL = 2;  // mS
int  messageOffset;

  
long last = millis();
long lastTime = millis();

void loop () 
  { 
   if((millis()- last)>1000*60*15){  //update weather every 15 minutes 
    getTime();
    last = millis();
   }
   if(s%5==0){
    updateTemp();
   }

 showData(); 
 updateTime();

 if(millis() < last) last = 0;
}  // end of loop


// =======================================================================
// retrive weather data

//const char *weatherHost = "api.openweathermap.org";




// =======================================================================

float utcOffset = 2;
long localEpoc = 0;
long localMillisAtUpdate = 0;

void getTime()
{
  WiFiClient client;
  if (!client.connect("www.google.co.th", 80)) {
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
      h = line.substring(23, 25).toInt()+5; 
      if(h>=24){
        h-=24;
        int TD = date.substring(5,7).toInt()+1;
        date = date.substring(0,5)+String(TD)+date.substring(7,date.length());
      }
      m = line.substring(26, 28).toInt();
      s = line.substring(29, 31).toInt();
      localMillisAtUpdate = millis();
      localEpoc = (h * 60 * 60 + m * 60 + s);
      String clock_date = date;
      int date_len = clock_date.length() + 1; 
      clock_date.toCharArray(date_message, date_len) ;
    
    }
  }
  client.stop();
}

// =======================================================================
void updateTemp(){
  sensors_event_t event;  
  dht.temperature().getEvent(&event);
  temp = event.temperature;
  dht.humidity().getEvent(&event);
  humidity = event.relative_humidity;
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
void showData(){
  if(millis()%100 == 0){
    lcd.clear();
  }
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
  String env = "T:" + String(temp)+"*c H:" + String(humidity)+"%";
  lcd.setCursor(2,2);
  lcd.print(env);
  lcd.setCursor(2,3);
  lcd.print(date);
}
