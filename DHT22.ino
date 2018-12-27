
#include "dht22.h"

using namespace sensors;

DHT22 test{ 14 };

void setup() {
  // put your setup code here, to run once:
  Serial.begin( 115200 );
  printDht( test );
  
}

void loop() {
  // put your main code here, to run repeatedly:
  int8_t* c = test.getBuffer();
  Serial.print( test.readSensor() );
  Serial.print( "\tBuffer[0]: " );
  Serial.print( c[0] );
  Serial.print( "\tBuffer[1]: " );
  Serial.print( c[1] );
  Serial.print( "\tBuffer[2]: " );
  Serial.print( c[2] );
  Serial.print( "\tBuffer[3]: " );
  Serial.print( c[3] );
  Serial.print( "\tBuffer[4]: " );
  Serial.println( c[4] );
  delay(500);
}


void printDht( DHT22& d )
{
  Serial.print( "DHT22:\tDataPin ");
  Serial.print( d.getDataPin() );
  Serial.println( " | " );
}
