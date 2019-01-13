//Global
#include <Arduino.h>
//END Global

//ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
//END ESP8266

#include <dht22.h>
#include <TSL256x.h>

#define SSID "NameOfNetwork"
#define SECURITY_KEY "AardvarkBadgerHedgehog"

#define URI "http://192.168.4.1/input.php"
#define auth "YouShallPass"

#define DHTPIN 0//10//2
#define DHTTYPE 22
#define DHTTHRESHHOLD 20 //24

#define SDA 4//D2
#define SCL 5//D1

#define ADDRESS 0x39//TSL I2C Address

using namespace sensors;

float humidity;
float temperatur;
float illumination;
int hasReadData;

ESP8266WiFiMulti WiFiMulti;
DHT22 dht(DHTPIN);
TSL256x tsl2561(ADDRESS, SDA, SCL);


void setupWiFi()
{
  Serial.println("SETUP ESP");
  for (uint8_t t = 3; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(SSID, SECURITY_KEY);
  Serial.println("DONE  ESP");
}


void setupDHT()
{
  Serial.println("Setup DHT");
  humidity = 0.0f;
  temperatur = 0.0f;
  Serial.println("Done  DHT");
}


void setupTSL()
{
  Serial.println("SETUP TSL");
  illumination = 0.0f;
  if(tsl2561.init())
  {
    Serial.printf("Part Number = %d\n", tsl2561.getPartNumber());
    Serial.printf("Revision Number = %d\n", tsl2561.getRevisionNumber());
    Serial.printf("Sensor Name: %s\n", tsl2561.getDeviceName());
    Serial.printf("Package Name: %s\n", tsl2561.getPackageName());
  }
  else
  {
    Serial.printf("Error Message: %s\n", tsl2561.getErrorMsg());
  }
  Serial.println("DONE  TSL");
}


void readWeatherData()
{
  //temperature and humidity
  DHT22::MeasureValues mv = dht.getTempAndHumi();
  humidity = mv.humidity;
  temperatur = mv.temperature;

  //illumination
  if(tsl2561.turnOn()) {
    if(!tsl2561.getLux(illumination))
    {
      hasReadData = 0;
    }
    else
    {
      hasReadData = 1;
    }
  }
  tsl2561.turnOff();
}

void sendWeatherData()
{
  int status = -1;
  //Try to establish WiFi connection
  status = WiFiMulti.run();
  for(int tries = 0; tries < 3 && status != WL_CONNECTED; tries++)
  {
    Serial.println("Could not establish Wifi Connection");
    Serial.printf("Errorcode = %d, retry in 3.3 sec.\n", status);
    delay(3333);
    status = WiFiMulti.run();
  }
  
  if (status == WL_CONNECTED)
  {
    HTTPClient http;
    String data;
    
    http.begin(URI);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    Serial.println("[HTTP] POST Request");
    if(hasReadData == 1)
    {
      data = String("authentification=") + auth + "&temperatur=" + temperatur + "&humidity=" + humidity + "&illumination=" + illumination;  
    }
    else
    {
      data = String("authentification=") + auth + "&DhtError=" + dht.getErrorMsg() + "&tslError=" + tsl2561.getErrorMsg();
    }
   
    Serial.println(data);
    int httpCode = http.POST(data);
    
    if (httpCode > 0) {
      Serial.printf("[HTTP] Status Code (%d)\n", httpCode);
      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] POST failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    
    http.end();
  }
}


void setup() {
  unsigned long measureTime = micros();
  Serial.begin( 9600 );
  Serial.setTimeout(2000);
  
  // Wait for serial to initialize.
  while (!Serial) { }
  
  Serial.println("\nI'm awake.");
  setupWiFi();
  setupDHT();
  setupTSL();
  hasReadData = 0;

  readWeatherData();
  if(hasReadData == 1)
  {
    sendWeatherData();  
  }
  
  Serial.println(dht.getErrorMsg());
  Serial.println("Going into deep sleep for 10 minutes");
  measureTime = micros() - measureTime;
  Serial.printf("measureTime: %ld", measureTime);

  //Deep sleep methods from user_interface.h
  system_deep_sleep_set_option(1);
  system_deep_sleep_instant( (60000*1000*10) - measureTime ); //10 minutes
  //system_deep_sleep_instant( (10000*1000) ); //10 sec
  //ESP.deepSleep( (10000*1000) - measureTime);
}

void loop() { }
