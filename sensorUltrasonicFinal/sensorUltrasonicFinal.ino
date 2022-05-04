#include "AsyncSonarLib.h"
#include "Constants.h"
#include <Arduino.h>
///#include <MPR121.h>
#include <Streaming.h>
#include <MIDI.h>

#define MAX_CONN_ATTEMPT 10
#define _BV(bit) (1 << (bit))

uint16_t lasttouched = 0;
uint16_t currtouched = 0;

uint16_t midiEspalda = 127;
uint16_t lastmidiEspalda = midiEspalda;
uint16_t rawEspalda = 0;

uint16_t midiFrente = 127;
uint16_t lastmidiFrente = midiFrente;
uint16_t rawFrente = 0;

//uint16_t touch_status = 0;
//uint16_t last_touch_status = touch_status;

uint16_t maxDistance = 2000;

MIDI_CREATE_DEFAULT_INSTANCE();

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

//MPR121 mpr121;
AsyncSonar sonarEspalda(A0, PingRecieved, TimeOut);
//  AsyncSonar sonarFrente(A1, PingRecieved2, TimeOut);




void setup()  {  
  Serial.begin(115200);
//  mprSetup();
  sonarEspalda.SetTemperatureCorrection(28);
  //  sonarFrente.SetTemperatureCorrection(28);
  sonarEspalda.Start(100);
  //  sonarFrente.Start(100);
  Serial.println("Iniciando sensores ultrasonicos...");
}

void loop() {
  sonarEspalda.Update(&sonarEspalda);
  // Convierte la informacion del sensor a una escala util para PD
   midiEspalda = map(rawEspalda, 10, maxDistance, 5, 127); // MIDI
  //  midiEspalda = logConvertion(rawEspalda, 10, maxDistance, 20, 20000);// Hertz
  if(midiEspalda != lastmidiEspalda) {
  //  Enviar datos de la distancia de la Espalda
    MIDI.sendNoteOn(midiEspalda, 100, 2);
    Serial.println(midiEspalda);
    lastmidiEspalda = midiEspalda;
  }

//  sonarFrente.Update(&sonarFrente);
//  midiFrente = map(rawFrente, 10, maxDistance, 5, 127);
//  if(midiFrente != lastmidiFrente) {
//    MIDI.sendNoteOn(midiEspalda, 100, 2);
//    lastmidiFrente = midiFrente;
//  }  


//  if (!mpr121.communicating(constants::device_address))
//  {
//    Serial << "mpr121 device not commmunicating!\n\n";
//    return;
//  }
//  mprRead();

}

//void mprSetup() {
//  
//  mpr121.setupSingleDevice(*constants::wire_ptr,
//    constants::device_address,
//    constants::fast_mode);
//
//  mpr121.setAllChannelsThresholds(constants::touch_threshold,
//    constants::release_threshold);
//  mpr121.setDebounce(constants::device_address,
//    constants::touch_debounce,  
//    constants::release_debounce);
//  mpr121.setBaselineTracking(constants::device_address,
//    constants::baseline_tracking);
//  mpr121.setChargeDischargeCurrent(constants::device_address,
//    constants::charge_discharge_current);
//  mpr121.setChargeDischargeTime(constants::device_address,
//    constants::charge_discharge_time);
//  mpr121.setFirstFilterIterations(constants::device_address,
//    constants::first_filter_iterations);
//  mpr121.setSecondFilterIterations(constants::device_address,
//    constants::second_filter_iterations);
//  mpr121.setSamplePeriod(constants::device_address,
//    constants::sample_period);
//  mpr121.startChannels(constants::physical_channel_count,
//    constants::proximity_mode);
//
//}
//
//void mprRead() {
//  currtouched = mpr121.getTouchStatus(constants::device_address);
//  uint8_t noteBase = 30;
//  for (uint8_t i=0; i < constants::physical_channel_count; i++) {
//    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
//    //  Enviar datos de la distancia de la Espalda
//      MIDI.sendNoteOn(noteBase + i, 120, 1);
//    }
//    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
//      MIDI.sendNoteOff(noteBase + i, 0, 1);
//    }
//  }
//
//  // reset our state
//  lasttouched = currtouched;
//}


uint16_t logConvertion(uint16_t position, uint16_t min, uint16_t max, uint16_t minLog, uint16_t maxLog) {
  uint16_t minp = min;
  uint16_t maxp = max;
  uint16_t minv = log(minLog);
  uint16_t maxv = log(maxLog);
  uint16_t scale = (maxv - minv) / (maxp - minp);
  return uint16_t(exp(minv + scale * (position - minp)));
}
