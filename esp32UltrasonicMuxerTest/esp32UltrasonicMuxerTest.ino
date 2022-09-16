#include "AsyncSonarLib.h"
#include "Constants.h"
#include <Arduino.h>

uint16_t rawEspalda = 0;
uint16_t lastRawEspalda = rawEspalda;

uint16_t rawFrente = 0;
uint16_t lastRawEspalda = midiFrente;

uint16_t maxDistance = 1000;


void PingRecieved(AsyncSonar& sonar)  {
  rawEspalda = sonar.GetMeasureMM();
  if(rawEspalda < 10) {
    rawEspalda = 10;
  } else if (rawEspalda > maxDistance) {
     rawEspalda = maxDistance;
  }
}

void PingRecieved2(AsyncSonar& sonar)  {
  rawFrente = sonar.GetMeasureMM();
  if(rawFrente < 10) {
    rawFrente = 10;
  } else if (rawFrente > maxDistance) {
    rawFrente = maxDistance;
  }
}

void TimeOut(AsyncSonar& sonar) {
  // Serial.println("TimeOut");
}

AsyncSonar sonarEspalda(A0, PingRecieved, TimeOut);
AsyncSonar sonarFrente(A1, PingRecieved2, TimeOut);

void setup()  {  
  Serial.begin(115200);
  sonarEspalda.SetTemperatureCorrection(28);
  sonarFrente.SetTemperatureCorrection(28);
  sonarEspalda.Start(100);
  sonarFrente.Start(100);
}

void loop() {
  sonarEspalda.Update(&sonarEspalda);
  // Convierte la informacion del sensor a una escala util para PD
  // midiEspalda = map(rawEspalda, 10, maxDistance, 5, 127); // MIDI
  // midiEspalda = logConvertion(rawEspalda, 10, maxDistance, 20, 20000);// Hertz
  if(rawEspalda != lastRawEspalda) {
  //  Enviar datos de la distancia de la Espalda
    //MIDI.sendNoteOn(midiEspalda, 100, 2);
    //Serial.println(midiEspalda); // Solo DEBUG, no usar porque el puerto MIDI usa la misma conexión
    lastRawEspalda = rawEspalda;
  }

  sonarFrente.Update(&sonarFrente);
  //midiFrente = map(rawFrente, 10, maxDistance, 5, 127);
  if(rawFrente != lastRawFrente) {
    //MIDI.sendNoteOn(rawFrente, 100, 1);
    lastRawFrente = rawFrente;
  }  

}


uint16_t logConvertion(uint16_t position, uint16_t min, uint16_t max, uint16_t minLog, uint16_t maxLog) {
  uint16_t minp = min;
  uint16_t maxp = max;
  uint16_t minv = log(minLog);
  uint16_t maxv = log(maxLog);
  uint16_t scale = (maxv - minv) / (maxp - minp);
  return uint16_t(exp(minv + scale * (position - minp)));
}
