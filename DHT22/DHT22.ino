
#include "dht22.h"
#include "DHT.h"

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

using namespace sensors;

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


void setup() {
  // put your setup code here, to run once:
  Serial.begin( 115200 );
  Serial.setTimeout(2000);
  
  // Wait for serial to initialize.
  while (!Serial) { }
  
  Serial.println("I'm awake.");
  myVersion();
  adafruitVersion();
  Serial.println("Going into deep sleep for 10 minutes");
  system_deep_sleep_set_option(1);
  system_deep_sleep_instant(60000*1000*10); //10 minutes
  //ESP.deepSleep(5e6); // 20e6 is 5 seconds
}

void loop() {
  // put your main code here, to run repeatedly:
}
