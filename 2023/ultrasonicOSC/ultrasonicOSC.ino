#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>

//  Configuracion from "Humedal" ---- ESP32 - Controller

//  Ultrasonicos HC-SR04
#define TRIG_PIN 12 
#define ECHO_PIN 13 //
float distances[1] ;    //  Arreglo para guardar la distancia del ultrasonico

char ssid[] = "HUMEDAL-VIVO";          // your network SSID (name)
char pass[] = "humedal212223";         // your network password
                                                                                                                                                                   
WiFiUDP Udp;                                
const IPAddress outIp(192,168, 0, 100);    //  remote IP -- RPI
const unsigned int outPort = 7777;          // remote port to receive OSC
const unsigned int localPort = 7778;        // local port to listen OSC 

void setup() {
  Serial.begin (115200);
  setSensors();
  connectWIFI();
}

void loop() {
  // Revisa por conexion WIFI
  if(WiFi.status() != WL_CONNECTED) { 
//    Serial.println("DESCONECTADO"); connectWIFI();
    }
  
  ultraMeasure();
  delay(10);
}


void ultraMeasure() {
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  float duration_us = pulseIn(ECHO_PIN, HIGH) * 0.017;
  
  //Serial.print("   Value: ");
  //Serial.println(duration_us);
  
  OSCMessage ultra("/ultra1");
  ultra.add(duration_us);
  Udp.beginPacket(outIp, outPort);
  ultra.send(Udp);
  Udp.endPacket();
  ultra.empty();
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

  //Serial.println("Starting UDP");
  //Udp.begin(localPort);
  //Serial.print("Local port: ");
  //Serial.println(localPort);
}


void setSensors(){
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.println("ULTRASONICO Inicializado");
}
