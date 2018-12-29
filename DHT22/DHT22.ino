
#include "dht22.h"
#include "DHT.h"

using namespace sensors;

DHT22 test{ 0 };
DHT test2( 0, 22, 20 );

void setup() {
  // put your setup code here, to run once:
  Serial.begin( 115200 );
  
}

void loop() {
  // put your main code here, to run repeatedly:
 
  static int i = 0;
  if( ++i % 2 == 0)
  {
  DHT22::MeasureValues mv = test.getTempAndHumi();
  Serial.print( "My: Temp=" );
  Serial.print( mv.temperature );
  Serial.print( " ,Humi=" );
  Serial.println( mv.humidity );
  }
  else 
  {
  test2.begin();
  Serial.print("Serious: Temp=");
  Serial.print(test2.readTemperature( false ));
  Serial.print(" ,Humi=");
  Serial.println(test2.readHumidity());
  
  }
  delay(2000);
}


void printDht( DHT22& d )
{
  Serial.print( "DHT22:\tDataPin ");
  Serial.print( d.getDataPin() );
  Serial.println( " | " );
}
