#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <analogWrite.h>

char ssid[] = "HUMEDAL-VIVO";          // your network SSID (name)
char pass[] = "humedal212223";                    // your network password  

//byte touchButtons[] = {12, 13, 14, 27};
byte touchButtons[] = {13};

const int numBut = sizeof(touchButtons);
const int ledPin = 18;   //19
const int tresholdMin = 10;
const int tresholdMax = 30;

//arrays to hold current and last state of buttons 
static byte lastState[numBut];
boolean currentState[numBut];
static byte  rawState[numBut];


WiFiUDP Udp;                                
const IPAddress outIp(192, 168, 50, 46);     // La IP remota de la RPI

const unsigned int outPort = 7777;          // remote port to receive OSC
const unsigned int localPort = 7778;        // local port to listen for OSC packets (actually not used for sending)



void setup() {
  // set up serial port
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  
  //debug
  Serial.print("OSC sender with ");
  Serial.print(numBut, DEC);
  Serial.println(" buttons");
 
  //set up button pins as inputs 
  for (int i=0; i<numBut; i++){
    pinMode(touchButtons[i], INPUT);
    currentState[i] = false;
    Serial.print("Button inputs ");
    Serial.println(i);
  }
  analogWrite(ledPin, 120);
}

void loop() {
  //if(WiFi.status() != WL_CONNECTED) {digitalWrite(ledPin, LOW);}
  checkButtons();
  delay(20);
}


void wifiSetup () {
    // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, pass);  //stops analogue read from happening on some pins!

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(ledPin,HIGH);
    Serial.print(".");
    delay(500);
    digitalWrite(ledPin,LOW);
    delay(500);
  }

  Serial.println("");
  digitalWrite(ledPin, HIGH);

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(localPort);
}

void checkButtons() {
  //int i = 4; //Prueba
  for (int i=0; i<numBut; i++){
    rawState[i] = touchRead(touchButtons[i]);
    if (rawState[i] <= tresholdMin) {
      currentState[i] = true;   
    } else if(rawState[i] >= tresholdMax) {  
      currentState[i] = false;
    }

    if(currentState[i] != lastState[i]) {
      sendMessageRPI(i, rawState[i]);
      lastState[i] = currentState[i];
    }
    Serial.print("Boton");
    Serial.print(i);
    Serial.print(" ");
    Serial.print(rawState[i]);
    Serial.print(" ");
    Serial.println(currentState[i]);
  }
  Serial.println("");
}

void sendMessageRPI(int button, byte state) {
   //send OSC message
   OSCMessage msg("/buttons/b");
   msg.add(button);
   msg.add(state);
   Udp.beginPacket(outIp, outPort);
   msg.send(Udp);
   Udp.endPacket();
   msg.empty();
}
