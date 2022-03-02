#include "AsyncSonarLib.h"
#include <Wire.h>
#include "Adafruit_MPR121.h"

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

uint16_t lasttouched = 0;
uint16_t currtouched = 0;
Adafruit_MPR121 cap = Adafruit_MPR121();

// ping complete callback
void PingRecieved(AsyncSonar& sonar)
{
  Serial.print("Milimeters: ");
  Serial.println(sonar.GetMeasureMM());
}

// timeout callback
void TimeOut(AsyncSonar& sonar)
{
  Serial.println("TimeOut");
}

AsyncSonar sonarA0(A0, PingRecieved, TimeOut);

// ---- In this demo, this code simulates other project tasks
unsigned long interval = 1000;
unsigned long previousMillis;

void debug(char* text)
{
  Serial.print(text);
  Serial.println(millis());
}


void setup()
{
  Serial.begin(115200);
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");
  sonarA0.SetTemperatureCorrection(28);  // optional
  sonarA0.Start(500); // start in 500ms

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
  return;

}
