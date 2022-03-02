/***************************************************
Copyright (c) 2018 Luis Llamas
(www.luisllamas.es)

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License
 ****************************************************/
 
#include "AsyncSonarLib.h"
#include <Wire.h>
#include "Adafruit_MPR121.h"

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

// You can have up to 4 on one i2c bus but one is enough for testing!
Adafruit_MPR121 cap = Adafruit_MPR121();

// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t lasttouched = 0;
uint16_t currtouched = 0;


// ping complete callback
void PingRecieved(AsyncSonar& sonar)
{
 Serial.print("Milimetros: ");
  Serial.println(sonar.GetMeasureMM());
}

// timeout callback
void TimeOut(AsyncSonar& sonar)
{
  Serial.println("TimeOut");
}

AsyncSonar sonarA0(A0, PingRecieved, TimeOut);

void setup()
{
  Serial.begin(115200);
   // Default address is 0x5A, if tied to 3.3V its 0x5B
   // If tied to SDA its 0x5C and if SCL then 0x5D
   if (!cap.begin(0x5A)) {
     Serial.println("MPR121 not found, check wiring?");
      while (1);
   }
  Serial.println("MPR121 found!");
  
  sonarA0.SetTemperatureCorrection(28);  // optional
  sonarA0.Start(100);  // start in 1500ms
}

void loop()
{
  // this is where magic begins
  sonarA0.Update(&sonarA0);

   currtouched = cap.touched();
  
  for (uint8_t i=0; i<12; i++) {
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

  // comment out this line for detailed data from the sensor!
  return;
  
  // debugging info, what
  Serial.print("\t\t\t\t\t\t\t\t\t\t\t\t\t 0x"); Serial.println(cap.touched(), HEX);
  Serial.print("Filt: ");
  for (uint8_t i=0; i<12; i++) {
    Serial.print(cap.filteredData(i)); Serial.print("\t");
  }
  Serial.println();
  Serial.print("Base: ");
  for (uint8_t i=0; i<12; i++) {
    Serial.print(cap.baselineData(i)); Serial.print("\t");
  }
  Serial.println();

  delay(10);
}
