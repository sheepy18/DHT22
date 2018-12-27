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

  void DHT22::resetBuffer()
  {
    buff[0] = buff[1] = buff[2] = buff[3] = buff[4] = 0;
  }

  bool DHT22::isCheckSumValid()
  {
    if( buff[4] == ((buff[0] + buff[1] + buff[2] + buff[3]) & 0xFF) )
      return true;

    return false;
  }

  bool DHT22::readDataBits()
  {    
    unsigned long start = micros();
    unsigned long startRX = micros();
    uint8_t i = 0;
    uint8_t rBit = 0;

    //start data transition bitwise
    for( i = 0; i < 40; ++i )
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
          //Higher bit first, 7(highest bit exponent) - (i%8)
          buff[(i / 8)] += exp2( 7 - (i % 8) ); 
      } 
     }   
     return true; 
  }
  
  DHT22::Error DHT22::readSensor()
  {
    resetBuffer();
    if( !set2Output() ) return 1;
    if( !startCommunication() ) return 2;
    if( !set2Input() ) return 3;
    if( !waitForInitalBits() ) return 4 ;
    if( !readDataBits() ) return 5;
    if( isCheckSumValid() ) return 6;
     
    return 0;
  }

  float DHT22::getTemperature()
  {
    float t = 0.f;
    t = buff[2];
    t += (buff[3] / 10.f);
    return t;
  }

  float DHT22::getHumidity()
  {
    float h = 0.f;
    h = buff[0];
    h += (buff[1] / 10.f);
    return h;
    //return 2 * h; may be the right solution or a close one, but why?
  }
}
