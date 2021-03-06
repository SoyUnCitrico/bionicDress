#include "AsyncSonarLib.h"
#include "Adafruit_MPR121.h"
#include <MIDI.h>
#include <Wire.h>

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

#define MAX_CONN_ATTEMPT 5

uint16_t lasttouched = 0;
uint16_t currtouched = 0;
uint16_t state = 5;
uint16_t lastState = state;

void PingRecieved(AsyncSonar& sonar)  {
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
  //MIDI.begin();
}

void loop() {
  sonarA0.Update(&sonarA0);

  if(state != lastState) {
    manejarEstado();
    lastState = state;
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

void manejarEstado() {
  switch(state) {
    case 0:
      // Serial.println("Muy Cerca");
      MIDI.sendNoteOn(20, 100, 2);
      break;
    case 1:
      // Serial.println("Cerca");
      MIDI.sendNoteOn(40, 100, 2);
      break;
    case 2:
      // Serial.println("Medio");
      MIDI.sendNoteOn(80, 100, 2);
      break;
    case 3:
      // Serial.println("Alejado");
      MIDI.sendNoteOn(110, 100, 2);
      break;
    default:
      Serial.println("Estado no definido");
      break;
  }
}
