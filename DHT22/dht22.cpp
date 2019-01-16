#include "dht22.h"

namespace sensors
{
  

  DHT22::MeasureValues DHT22::getTempAndHumi()
  {    
    DHT22::Error e = readSensor();
    
    if( e )
    {
      switch(e)
      {
        case 1:
          err = "Error DHT22: Couldn´t set pin to output";
          break;
        case 2:
          err = "Error DHT22: Couldn´t start communication";
          break;
        case 3:
          err = "Error DHT22: Couldn´t set pin to input";
          break;
        case 4:
          err = "Error DHT22: Couldn´t receive the startbits";
          break;
        case 5:
          err = "Error DHT22: Couldn´t receive data bits";
          break;
        case 6:
          err = "Error DHT22: Detected invalid checksum";
          break;        
        default:
          err = "Error DHT22: Something went wrong! Couldn´t receive any data";
          break;
      }
      return DHT22::MeasureValues{ 0, 0, false};
    }
    
    
    return DHT22::MeasureValues{ getTemperature(), getHumidity(),true };
  }
  
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
    //wait 1 milisec on low voltage as a startsignal 
    digitalWrite( data, LOW );
    delay(1);
    noInterrupts();
    digitalWrite( data , HIGH);
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
    return true;
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

  bool DHT22::readDataBitsAlternative()
  {
    uint8_t j = 0;
    uint8_t rBit = 0;

    for( uint8_t i = 0; i < 40; ++i)
    {
      j = 0;
      while( digitalRead( data ) == LOW)
      {
        delayMicroseconds(1);
      }
      while( digitalRead( data ) == HIGH )
      {
        ++j;
        delayMicroseconds(1);
      }

      if( j > 30 )
      {
        buff[(i / 8)] += exp2( 7 - (i % 8) ); 
        readedBits[i] = '1';
      }
      else
      {
         readedBits[i] = '0';
      }
    }

    return true;
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
      
      if( micros() - startRX > 40 )
      {              
          //Higher bit first, 7(highest bit exponent) - (i%8)
          buff[(i / 8)] += exp2( 7 - (i % 8) ); 
          readedBits[i] = '1';
      } else {
        readedBits[i] = '0';
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
    if( !isCheckSumValid() ) return 6;     
    return 0;
  }
  
  DHT22::Value DHT22::getTemperature()
  {
    float t = 0.f;
    t = buff[2];
    t += (buff[3] / 10.f);
    return t;
  }

  DHT22::Value DHT22::getHumidity()
  {
    float h2 = 0.f;
    h2 = buff[0];
    h2 *= 256;
    h2 += buff[1];
    h2 /= 10;
    return h2;
  }
  
}
