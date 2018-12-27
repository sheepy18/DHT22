#include "dht22.h";

namespace sensors
{
  /*
   * used for computing bits into integer
   */
  uint8_t exp2( uint8_t e )
  {
    uint8_t f = 1;
    if( e == 0)
      return 1;
    for( int i = 0; i < e; ++i)
    {
      f *= 2;
    }
    return f;
  }

  bool DHT22::set2Output()
  {
   //set datapin as output on high voltage
    pinMode( data, OUTPUT );
    digitalWrite( data, HIGH );
    delay(20); 
    return true;
  }

  bool DHT22::startCommunication()
  {
    //wait 2 milisec on low voltage as a startsignal 
    digitalWrite( data, LOW );
    delay(2);
    return true;
  }

  bool DHT22::set2Input()
  {
    //set datapin on input and wait for measuring ca.20 micro seconds
    pinMode( data, INPUT );
    delayMicroseconds(20);  
    return true;
  }

  bool DHT22::waitForInitalBits()
  {
    //initalise variables for time measuring and for read one bit(=high/low voltage)
    unsigned long start = micros(); 
    uint8_t rBit = 0;
    
    //first 80us startsignal on LOW
    rBit = digitalRead( data );
    while( rBit != HIGH )
    {
      rBit = digitalRead( data ); 
      //time measuring to prevent infinte loops if something went wrong
      if( (micros() - start) >= 5000 )
        return false;
    }
    
    //second 80us startsignal on HIGH
    rBit = digitalRead( data );
    while( rBit == HIGH )
    {  
      rBit = digitalRead( data );
      if( (micros() - start) >= 5000 )
        return false;
    }
  }

  bool DHT22::readDataBits()
  {
    buffer[0] = buffer[1] = buffer[2] = buffer[3] = buffer[4] = 0;
    unsigned long start = micros();
    unsigned long startRX = micros();
    uint8_t i = 0;
    uint8_t rBit = 0;

    //start data transition bitwise
    for( i = 1; i <= 40; ++i )
    {
      rBit = digitalRead( data );
      while( rBit == LOW )
      {
        rBit = digitalRead( data );
        if( micros() - start >= 5000 )
          return false;
      }  
      startRX = micros();
      while( rBit == HIGH )
      {
        rBit = digitalRead( data );
        if( micros() - start >= 5000 )
          return false;
      }
      
      if( micros() - startRX > 30 )
      {
        //vorzeichenbehaftet letztes bit entscheidet es
         //if( i % 8 == 7)
           //buffer[ (i / 8) % 5 ] *= -1;  
               
          //Higher bit first and signed, 6(highest bit exponent) - (i%7)
          buffer[ (i / 8) % 5 ] += exp2( (6) - (i % 7) );   
      } 
     }   
     return true; 
  }

  
  
  DHT22::Error DHT22::readSensor()
  {
    if( !set2Output() ) return 1;
    if( !startCommunication() ) return 2;
    if( !set2Input() ) return 3;
    if( !waitForInitalBits() ) return 4 ;
    if( !readDataBits() ) return 5;
     
    return 0;
  }
}
