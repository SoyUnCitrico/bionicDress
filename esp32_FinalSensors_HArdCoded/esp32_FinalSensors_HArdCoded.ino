#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>

//  Configuracion from "Humedal" ---- ESP32 - Controller
//  Muxer CD4051
#define PINA 16   //  A 
#define PINB 17   //  B 
#define PINC 5    //  C 
#define INCOM 34  // Com In/Out 
#define NUM_SENS 7  // Sensores análogos
//  Ultrasonicos HC-SR04
#define TRIG_PIN 12 
#define ECHO_PIN 13 //
#define TRIG_PIN2 26 
#define ECHO_PIN2 27 

int muxChannel[7][3]={
    {0,0,0}, //channel 0
    {1,0,0}, //channel 1
    {0,1,0}, //channel 2
    {1,1,0}, //channel 3
    {0,0,1}, //channel 4
    {1,0,1}, //channel 5
    {0,1,1}, //channel 6
};
int muxes[NUM_SENS];    //  Arreglo para guardar los resultados del muxer CD4051
float distances[2] ;    //  Arreglo para guardar la distancia del ultrasonico

char ssid[] = "VENDO AGUA";          // your network SSID (name)
char pass[] = "simon&mollY";         // your network password

WiFiUDP Udp;                                
const IPAddress outIp(192, 168,88, 240);    //  remote IP -- RPI
const unsigned int outPort = 7777;          // remote port to receive OSC
const unsigned int localPort = 7778;        // local port to listen OSC 

void setup() {
  Serial.begin (115200);
  setSensors();
  connectWIFI();
}

void loop() {
  // Revisa por conexion WIFI
  if(WiFi.status() != WL_CONNECTED) { Serial.println("DESCONECTADO"); connectWIFI();}
  
  //ultraMeasureOne();
  //ultraMeasureTwo();
  readAllMux();
  delay(10);
}


void ultraMeasureOne() {
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  float duration_us = pulseIn(ECHO_PIN, HIGH) * 0.017;
  OSCMessage ultra("/ultra1");
  ultra.add(duration_us);
  Udp.beginPacket(outIp, outPort);
  ultra.send(Udp);
  Udp.endPacket();
  ultra.empty();
}

void ultraMeasureTwo() {
  digitalWrite(TRIG_PIN2, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN2, LOW);
  float duration_us2 = pulseIn(ECHO_PIN2, HIGH) * 0.017;
  OSCMessage ultra2("/ultra2");
  ultra2.add(duration_us2);
  Udp.beginPacket(outIp, outPort);
  ultra2.send(Udp);
  Udp.endPacket();
  ultra2.empty();
}

int readPinMux(int channel){
  int controlPin[] = {PINA, PINB, PINC};
  for(int i = 0; i < 3; i ++){
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }
  int val = analogRead(INCOM);
  return val;
}

void readAllMux(){
//    OSCMessage msg("/muxer1");
//    msg.add(readPinMux(0));
//    Udp.beginPacket(outIp, outPort);
//    msg.send(Udp);
//    Udp.endPacket();
//    msg.empty();
//
//    
//    OSCMessage msg2("/muxer2");
//    msg2.add(readPinMux(1));
//    Udp.beginPacket(outIp, outPort);
//    msg2.send(Udp);
//    Udp.endPacket();
//    msg2.empty();
//    
//    OSCMessage msg3("/muxer3");
//    msg3.add(readPinMux(2));
//    Udp.beginPacket(outIp, outPort);
//    msg3.send(Udp);
//    Udp.endPacket();
//    msg3.empty();
//    
//    OSCMessage msg4("/muxer4");
//    msg4.add(readPinMux(3));
//    Udp.beginPacket(outIp, outPort);
//    msg4.send(Udp);
//    Udp.endPacket();
//    msg4.empty();
//    
//    OSCMessage msg5("/muxer5");
//    msg5.add(readPinMux(4));
//    Udp.beginPacket(outIp, outPort);
//    msg5.send(Udp);
//    Udp.endPacket();
//    msg5.empty();
//    
//    OSCMessage msg6("/muxer6");
//    msg6.add(readPinMux(5));
//    Udp.beginPacket(outIp, outPort);
//    msg6.send(Udp);
//    Udp.endPacket();
//    msg6.empty();
    
    OSCMessage msg7("/muxer7");
    int actualRead = readPinMux(6); 
    msg7.add(actualRead);
    Udp.beginPacket(outIp, outPort);
    msg7.send(Udp);
    Udp.endPacket();
    msg7 .empty();
 }

void logInfoMuxer() {
  Serial.print("{ ");
  for(int i = 0; i < NUM_SENS; i++) {
    Serial.print(readPinMux(i));
    Serial.print(" --- ");
  }
  Serial.print(" } ");
  Serial.println("");  
}

void logInfoUltrasonic() {
  for(int i = 0; i < sizeof(distances)-1; i++){
    Serial.print("Ultrasonic: ");
    Serial.print(i);
    Serial.print("   Value: ");
    Serial.println(distances[i]);
  }
  Serial.println("");
}


void connectWIFI() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(localPort);
}


void setSensors(){
  pinMode(PINA, OUTPUT); 
  pinMode(PINB, OUTPUT); 
  pinMode(PINC, OUTPUT); 

  digitalWrite(PINA, LOW);
  digitalWrite(PINB, LOW);
  digitalWrite(PINC, LOW);
  Serial.println("MUXER Inicializado");

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  pinMode(TRIG_PIN2, OUTPUT);
  pinMode(ECHO_PIN2, INPUT);
  Serial.println("ULTRASONICO Inicializado");

}
