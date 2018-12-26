#include "dht22.h";

namespace sensors
{

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
  
  uint8_t Dht22::readSensor()
  {
    buffer[0] = buffer[1] = buffer[2] = buffer[3] = buffer[4] = 0;
    unsigned long start = micros();
    unsigned long startRX = micros();
    uint8_t i = 0;
    uint8_t rBit = 0;
    
    //erstmal datenpin auf High setzen
    pinMode( data, OUTPUT );
    digitalWrite( data, HIGH );
    delay(20);
    
    //Mitteilen, dass ich kommunizieren möchte
    digitalWrite( data, LOW );
    delay(1);
    
    //auf input stellen und warten bis gemessen wurde
    pinMode( data, INPUT );
    delayMicroseconds(20);    

    //erste 80us startsignal auf LOW
    rBit = digitalRead( data );
    while( rBit != HIGH )
    {
      rBit = digitalRead( data ); 
      //Serial.println( rBit );
      if( (micros() - start) >= 5000 )
        return 1;
    }
    
    //zweite 80us startsignal auf HIGH
    rBit = digitalRead( data );
    start = micros();
    while( rBit == HIGH )
    {  
      rBit = digitalRead( data );
      if( (micros() - start) >= 5000 )
        return 1;
    }

    //start der bit-weisen Datenübertragung
    start = micros();
    for( i = 1; i <= 40; ++i )
    {
      rBit = digitalRead( data );
      while( rBit == LOW )
      {
        rBit = digitalRead( data );
        if( micros() - start >= 5000 )
          return 2;
      }  
      startRX = micros();
      while( rBit == HIGH )
      {
        rBit = digitalRead( data );
        if( micros() - start >= 5000 )
          return 3;
      }
      
      if( micros() - startRX > 30 )
      {
        
        //vorzeichenbehaftet letztes bit entscheidet es
         if( i % 8 == 7)
          //buffer[ (i / 8) % 5 ] *= -1;  
          ;        
         else
          //Higher bit first und vorzeichenbehaftet deswegen, 6(höchstes Bit) - (i%7) 
          buffer[ (i / 8) % 5 ] += exp2( (6) - (i % 7) );  
          
      }
      
      
     }
     
    return 0;
  }
}
