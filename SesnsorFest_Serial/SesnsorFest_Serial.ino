#include "AsyncSonarLib.h"
#include "Constants.h"
#include <Arduino.h>
#include <MPR121.h>
#include <Streaming.h>
#include <MIDI.h>

#define MAX_CONN_ATTEMPT 10

uint16_t lasttouched = 0;
uint16_t currtouched = 0;

uint16_t midiEspalda = 127;
uint16_t lastmidiEspalda = midiEspalda;
uint16_t rawEspalda = 0;

uint16_t midiFrente = 127;
uint16_t lastmidiFrente = midiFrente;
uint16_t rawFrente = 0;

uint16_t touch_status = 0;
uint16_t last_touch_status = touch_status;

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

MPR121 mpr121;
AsyncSonar sonarEspalda(A0, PingRecieved, TimeOut);
AsyncSonar sonarFrente(A1, PingRecieved2, TimeOut);




void setup()  {  
  Serial.begin(115200);
  mpr121.setupSingleDevice(*constants::wire_ptr,
    constants::device_address,
    constants::fast_mode);

  mpr121.setAllChannelsThresholds(constants::touch_threshold,
    constants::release_threshold);
  mpr121.setDebounce(constants::device_address,
    constants::touch_debounce,  
    constants::release_debounce);
  mpr121.setBaselineTracking(constants::device_address,
    constants::baseline_tracking);
  mpr121.setChargeDischargeCurrent(constants::device_address,
    constants::charge_discharge_current);
  mpr121.setChargeDischargeTime(constants::device_address,
    constants::charge_discharge_time);
  mpr121.setFirstFilterIterations(constants::device_address,
    constants::first_filter_iterations);
  mpr121.setSecondFilterIterations(constants::device_address,
    constants::second_filter_iterations);
  mpr121.setSamplePeriod(constants::device_address,
    constants::sample_period);
  mpr121.startChannels(constants::physical_channel_count,
    constants::proximity_mode);
    
  sonarEspalda.SetTemperatureCorrection(28);
  sonarFrente.SetTemperatureCorrection(28);
  sonarEspalda.Start(100);
  sonarFrente.Start(100);
}

void loop() {
  sonarEspalda.Update(&sonarEspalda);

  midiEspalda = map(rawEspalda, 10, maxDistance, 5, 127);
  if(midiEspalda != lastmidiEspalda) {
//    MIDI.sendNoteOn(midiEspalda, 100, 2);
    Serial.print("Espalda:");
    Serial.println(midiEspalda);
    lastmidiEspalda = midiEspalda;
  }

  sonarFrente.Update(&sonarFrente);
  midiFrente = map(rawFrente, 10, maxDistance, 5, 127);
  if(midiFrente != lastmidiFrente) {
//    MIDI.sendNoteOn(midiEspalda, 100, 2);
    Serial.print("Frente:");
    Serial.println(midiFrente);
    lastmidiFrente = midiFrente;
  }  

//  delay(constants::loop_delay);
  if (!mpr121.communicating(constants::device_address))
  {
    Serial << "mpr121 device not commmunicating!\n\n";
    return;
  }
  
  touch_status = mpr121.getTouchStatus(constants::device_address);
  if(touch_status != last_touch_status) {
    Serial << "touch_status: " << _BIN(touch_status) << "\n";
    last_touch_status = touch_status;
  }

//  for(int i = 0; i < constants::physical_channel_count; i++) {
//      bool channel_touched = mpr121.channelTouched(i);
//      Serial << "Sensor" << i << ": " << channel_touched << "\n";
//  }

}
