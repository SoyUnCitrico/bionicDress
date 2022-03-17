#include <MIDI.h>
#include "AsyncSonarLib.h"

MIDI_CREATE_DEFAULT_INSTANCE();

int ledPin = 13;
int saved;

void PingRecieved(AsyncSonar& sonar)
{
  saved = sonar.GetMeasureMM();
  Serial.print("mm: ");
  Serial.println(saved);
}

void TimeOut(AsyncSonar& sonar)
{
  Serial.println("TimeOut");
}

AsyncSonar sonarA0(A0, PingRecieved, TimeOut);

void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  sonarA0.SetTemperatureCorrection(28);  // optional
  sonarA0.Start(500); // start in 500ms
}

void loop() {
  sonarA0.Update(&sonarA0);
  MIDI.sendNoteOn(42, 127, 1);    // Send a Note (pitch 42, velo 127 on channel 1)
  delay(500);		            // Wait for a second
  MIDI.sendNoteOff(42, 0, 1);
  delay(1000);
}
