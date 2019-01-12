
#include "dht22.h"
#include "DHT.h"

//Global
#include <Arduino.h>
//END Global

//ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
//END ESP8266

//DHT
#include "dht22.h"
//END DHT

//TSL2561
//#include <Wire.h>
//#include "Adafruit_Sensor.h"
//#include "Adafruit_TSL2561_U.h"
//END TSL2561

//#define SSID "UPC7603911"
#define SSID "NameOfNetwork"
#define SECURITY_KEY "AardvarkBadgerHedgehog"

#define URI "http://192.168.4.1/input.php"
#define auth "YouShallPass"

#define DHTPIN 0//10//2
#define DHTTYPE 22
#define DHTTHRESHHOLD 20 //24

using namespace sensors;

float humidity;
float temperatur;
float illumination;
int hasReadData;

ESP8266WiFiMulti WiFiMulti;
DHT22 dht(DHTPIN);
//Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

DHT22 test{ 0 };
DHT test2( 0, 22, 20 );


void myVersion()
{
  DHT22::MeasureValues mv = test.getTempAndHumi();
  Serial.print( "My: Temp=" );
  Serial.print( mv.temperature );
  Serial.print( " ,Humi=" );
  Serial.println( mv.humidity );
  delay(1000);
}

void adafruitVersion()
{
  test2.begin();
  Serial.print("Serious: Temp=");
  Serial.print(test2.readTemperature( false ));
  Serial.print(" ,Humi=");
  Serial.println(test2.readHumidity());
}


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

void configureSensor(void)
{
  /* You can also manually set the gain or enable auto-gain support */
  //tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  //tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  //tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
  
  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
 // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

  /* Update these values depending on what you've set above! */  
  /*Serial.println("------------------------------------");
  Serial.print  ("Gain:         "); Serial.println("Auto");
  Serial.print  ("Timing:       "); Serial.println("13 ms");
  Serial.println("------------------------------------");*/
}
/*
void setupTSL()
{
  Serial.println("SETUP TSL");
  illumination = 0.0f;
  if(!tsl.begin())
  {
    Serial.println("ERROR No TSL2561 detected!");
  }
  else
  {
    configureSensor();
  }
  Serial.println("DONE  TSL");
}
*/

void readWeatherData()
{
  //temperature and humidity
  DHT22::MeasureValues mv = dht.getTempAndHumi();
  humidity = mv.humidity;
  temperatur = mv.temperature;
  /*if (isnan(humidity) || isnan(temperatur)) {
    Serial.println("DHT: Failed to read from sensor!");
    hasReadData = 0;
    return;
  }
  else
  {
    hasReadData = 1;
  }*/
  //readLightData();
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
    
    http.begin(URI);//URI
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    Serial.println("[HTTP] POST Request");
    String data = String("authentification=") + auth + "&temperatur=" + temperatur + "&humidity=" + humidity + "&illumination=" + illumination;
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
  // put your setup code here, to run once:
  unsigned long measureTime = micros();
  Serial.begin( 9600 );
  Serial.setTimeout(2000);
  
  // Wait for serial to initialize.
  while (!Serial) { }
  
  Serial.println("\nI'm awake.");
  setupWiFi();
  setupDHT();
  //setupTSL();
  hasReadData = 0;

  readWeatherData();
  sendWeatherData();
  
  Serial.println("Going into deep sleep for 10 minutes");
  measureTime = micros() - measureTime;
  Serial.printf("measureTime: %ld",measureTime);

  //Deep sleep methods from user_interface.h
  system_deep_sleep_set_option(1);
  system_deep_sleep_instant( (60000*1000*10) - measureTime ); //10 minutes
  //system_deep_sleep_instant( (10000*1000) - measureTime ); //10 sec
}




void loop() {
  // put your main code here, to run repeatedly:
}
