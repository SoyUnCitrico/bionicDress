//#include <WiFi.h>
//#include <WiFiUdp.h>
#include <OSCMessage.h>

#define PINA 16
#define PINB 17
#define PINC 5
#define INCOM 34
#define NUM_SENS 7
#define TRIG_PIN 12 // TRIG pin
#define ECHO_PIN 13 // ECHO pin
#define TRIG_PIN2 26 // TRIG pin
#define ECHO_PIN2 27 // ECHO pin

#define MAX_LIMIT 1500
#define MIN_LIMIT 10 

float distance; // store the distance from sensor
float distance2; // store the distance from sensor

int muxChannel[7][3]={
    {0,0,0}, //channel 0
    {1,0,0}, //channel 1
    {0,1,0}, //channel 2
    {1,1,0}, //channel 3
    {0,0,1}, //channel 4
    {1,0,1}, //channel 5
    {0,1,1}, //channel 6
};
int muxes[NUM_SENS];
//char ssid[] = "humedalTraje";          // your network SSID (name)
//char pass[] = "humedal1234";                    // your network password

//WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
//const IPAddress outIp(192, 168, 0, 101);     // remote IP of your computer
//const unsigned int outPort = 7777;          // remote port to receive OSC
//const unsigned int localPort = 7778;        // local port to listen for OSC packets (actually not used for sending)

void setup() {
  // begin serial port
  Serial.begin (115200);

  // configure the trigger and echo pins to output mode
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  pinMode(TRIG_PIN2, OUTPUT);
  pinMode(ECHO_PIN2, INPUT);
  setMux();
}

void loop() {
  float * measures = ultrasonicMeasure();
  for(int i = 0; i < sizeof(measures)-1; i++){
    Serial.print("Ultrasonic: ");
    Serial.print(i);
    Serial.print("   Value: ");
    Serial.println(measures[i]);
  }
  Serial.println("");
  allMux();
}

float * ultrasonicMeasure() {
  // generate 10-microsecond pulse to TRIG pin
  digitalWrite(TRIG_PIN, HIGH);
  digitalWrite(TRIG_PIN2, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  digitalWrite(TRIG_PIN2, LOW);

  // measure duration of pulse from ECHO pin
  float duration_us = pulseIn(ECHO_PIN, HIGH);
  float duration_us2 = pulseIn(ECHO_PIN2, HIGH);

  float distances[] = {(0.017 * duration_us), (0.017 * duration_us2)};
  return distances;
}


void sendMessageRPI(int sensor, int state) {
   //send OSC message
   OSCMessage msg("/sensor/s");
   msg.add(sensor);
   msg.add(state);
   Udp.beginPacket(outIp, outPort);
   msg.send(Udp);
   Udp.endPacket();
   msg.empty();
//}

void setMux(){
  pinMode(PINA, OUTPUT); 
  pinMode(PINB, OUTPUT); 
  pinMode(PINC, OUTPUT); 

  digitalWrite(PINA, LOW);
  digitalWrite(PINB, LOW);
  digitalWrite(PINC, LOW);
  Serial.println("MUXER Inicializado");
}

int readMux(int channel){
  int controlPin[] = {PINA, PINB, PINC};
  for(int i = 0; i < 3; i ++){
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }
  int val = analogRead(INCOM);
  return val;
}

void allMux(){
  for(int i = 0; i < NUM_SENS; i++) {
    muxes[i] = readMux(i);
  }
  delay(10);
  logInfo();
  //return muxes;
}

void logInfo() {
  Serial.print("{ ");
  for(int i = 0; i < NUM_SENS; i++) {
    Serial.print(muxes[i]);
    Serial.print(" --- ");
  }
  Serial.print(" } ");
  Serial.println("");  
}
