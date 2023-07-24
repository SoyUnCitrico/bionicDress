#include "AsyncSonarLib.h"
#include "Adafruit_MPR121.h"
#include <MIDI.h>
#include <Wire.h>

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

#define MAX_CONN_ATTEMPT 10

uint16_t lasttouched = 0;
uint16_t currtouched = 0;
uint16_t cutFrequency = 127;
uint16_t lastCutFrequency = cutFrequency;
uint16_t measure = 0;
uint16_t maxDistance = 1000;

void PingRecieved(AsyncSonar& sonar)  {
  measure = sonar.GetMeasureMM();
  if(measure < 10) {
    measure = 10;
  } else if (measure > maxDistance) {
    measure = maxDistance;
  }
}

void TimeOut(AsyncSonar& sonar)
{
  // Serial.println("TimeOut");
}

MIDI_CREATE_DEFAULT_INSTANCE();
Adafruit_MPR121 cap = Adafruit_MPR121();
AsyncSonar sonarA0(A1, PingRecieved, TimeOut);

void setup()  {
  sonarA0.SetTemperatureCorrection(28);
  sonarA0.Start(100);
  
  Serial.begin(115200);
  
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
//  MIDI.begin(115200);
}

void loop() {
  sonarA0.Update(&sonarA0);

  cutFrequency = map(measure, 10, maxDistance, 5, 127);
  if(cutFrequency != lastCutFrequency) {
    MIDI.sendNoteOn(cutFrequency, 100, 2);
    lastCutFrequency = cutFrequency;
  }
  
  currtouched = cap.touched(); 
  for (int i=0; i<6; i++) {
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
      int note = 40 + i;
      MIDI.sendNoteOn(note, 100, 1);
      //Serial.print(i); Serial.println(" Tocado");
    }
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
      int note = 40 + i;
      MIDI.sendNoteOff(note, 0, 1);
    }
  }
  lasttouched = currtouched;
}
