#include <WiFi.h>.
#include <WiFiUdp.h>
#include <OSCMessage.h>

char ssid[] = "Stalingrado";          // your network SSID (name)
char pass[] = "Qu1nt0s3n0r10";                    // your network password
byte touchButtons[] = {4, 15, 13, 12, 27};
const int numBut = sizeof(touchButtons);
const int ledPin = 23;
const int treshold = 8;

//arrays to hold current and last state of buttons 
static byte lastState[numBut];
static byte currentState[numBut];


WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
const IPAddress outIp(192, 168, 100, 152);     // remote IP of your computer
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
  //  for (int i=0; i<numBut; i++){
  //    pinMode(buttons[i], INPUT);
  //    Serial.print("Button inputs ");
  //    Serial.println(i);
  //  }
  
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

void loop() {
  if(WiFi.status() != WL_CONNECTED) {digitalWrite(ledPin, LOW);}
  checkButtons();
  delay(10);
}

void checkButtons() {
  //start a for loop
  for (int i=0; i<numBut; i++){
    // read the button pins and fill current array
    currentState[i] = touchRead(touchButtons[i]);
    
    //compare each index of the current and last array
    if (currentState[i] < treshold){  //if they are not the same as last time
    //if (currentState[i] != lastState[i]){  //if they are not the same as last time
        //print them out
        Serial.print("Boton Activado");
        Serial.println(i);
        //Serial.print(" ");
        //Serial.println(currentState[i]);
        
        //send OSC message
        OSCMessage msg("/buttons/b");
        msg.add(i);
        msg.add(currentState[i]);
        Udp.beginPacket(outIp, outPort);
        msg.send(Udp);
        Udp.endPacket();
        msg.empty();
        
        //overwrite the last state with the current one
        lastState[i] = currentState[i];      
    }
    //Serial.println("");
  }
}


//
//void setup()
//{
//  Serial.begin(115200);
//  delay(1000); // give me time to bring up serial monitor
//}
//
//void loop()
//{
//  Serial.print("T0: ");
//  Serial.println(touchRead(T0));  
//  Serial.print("T7: ");
//  Serial.println(touchRead(T7));  
//  Serial.print("T3: ");
//  Serial.println(touchRead(T3));
//  Serial.print("T4: ");
//  Serial.println(touchRead(T4));
//  Serial.print("T5: ");
//  Serial.println(touchRead(T5));
//  Serial.println();
//  delay(200);
//}
