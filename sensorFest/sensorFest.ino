#include "AsyncSonarLib.h"
#include <Wire.h>
#include "Adafruit_MPR121.h"

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

#define MAX_CONN_ATTEMPT 5



uint16_t lasttouched = 0;
uint16_t currtouched = 0;
uint16_t state = 5;
uint16_t lastState = state;

void PingRecieved(AsyncSonar& sonar)
{
//  Serial.print("Milimetros: ");
//  Serial.println(sonar.GetMeasureMM());
  float measure = sonar.GetMeasureMM();
  
  if(measure < 100) {
    state = 0;
  } else if (measure >= 200 && measure <= 1000){
    state = 1;
  } else if (measure > 1000 && measure <= 2000){
    state = 2;
  } else if (measure > 2000) {
    state = 3;
  }
}


void TimeOut(AsyncSonar& sonar)
{
  // Serial.println("TimeOut");
}

Adafruit_MPR121 cap = Adafruit_MPR121();
AsyncSonar sonarA0(A1, PingRecieved, TimeOut);

void setup()
{
  Serial.begin(115200);
   // Default address is 0x5A, if tied to 3.3V its 0x5B
   // If tied to SDA its 0x5C and if SCL then 0x5D
   Serial.println("");
   Serial.print("Conectanto al MPR-121");
   bool isInit = cap.begin(0x5A);
   while (!isInit) {
     Serial.print(".");
     isInit = cap.begin(0x5A);
     delay(500);
   }
  Serial.println("");
  Serial.println("MPR121 Encontrado!");
  
  sonarA0.SetTemperatureCorrection(28);  // optional
  sonarA0.Start(100);  // start in 1500ms
}

void loop()
{
  
  sonarA0.Update(&sonarA0);
  if(state != lastState) {
    imprimeInfo();
    lastState = state;
  }
  currtouched = cap.touched(); 
  for (uint8_t i=0; i<6; i++) {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
      Serial.print(i); Serial.println(" touched");
    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
      Serial.print(i); Serial.println(" released");
    }
  }

  // reset our state
  lasttouched = currtouched;

}

void imprimeInfo() {
  switch(state) {
    case 0:
      Serial.println("Muy Cerca");
      break;
    case 1:
      Serial.println("Cerca");
      break;
    case 2:
      Serial.println("Medio");
      break;
    case 3:
      Serial.println("Alejado");
      break;
    default:
      Serial.println("Estado no definido");
      break;
  }
}
