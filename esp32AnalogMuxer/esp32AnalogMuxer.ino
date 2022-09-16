#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>

#define PINA 16
#define PINB 17
#define PINC 5
#define INCOM 34
#define NUM_SENS 7

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
char ssid[] = "humedalTraje";          // your network SSID (name)
char pass[] = "humedal1234";                    // your network password

WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
const IPAddress outIp(192, 168, 0, 101);     // remote IP of your computer
const unsigned int outPort = 7777;          // remote port to receive OSC
const unsigned int localPort = 7778;        // local port to listen for OSC packets (actually not used for sending)



void setup() {
  Serial.begin(115200);
  
//  // Connect to WiFi network
//  Serial.println();
//  Serial.println();
//  Serial.print("Connecting to ");
//  Serial.println(ssid);
//  
//  WiFi.begin(ssid, pass);  //stops analogue read from happening on some pins!
//
//  while (WiFi.status() != WL_CONNECTED) {
//    Serial.print(".");
//    delay(500);
//  }
//  Serial.println("");
//
//  Serial.println("WiFi connected");
//  Serial.println("IP address: ");
//  Serial.println(WiFi.localIP());
//
//  Serial.println("Starting UDP");
//  Udp.begin(localPort);
//  Serial.print("Local port: ");
//  Serial.println(localPort);

  setMux();
  
}

void loop() {
//  if(WiFi.status() != WL_CONNECTED) {
//    Serial.println("DESCONECTADO");
//   }

  
    
    //sendMessageRPI(i, val);
    allMux();
    delay(100);
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
}

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
