#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <analogWrite.h>

//char ssid[] = "HUMEDAL-VIVO";          
//char pass[] = "humedal212223";                     
char ssid[] = "VENDO AGUA";          // your network SSID (name)
char pass[] = "simon&mollY";                    // your network password  

byte touchButtons[] = {12, 13, 14, 27};
//byte touchButtons[] = {13};

const int numBut = sizeof(touchButtons);
const int ledPin = 18;
const int buttonPin = 19;
const int tresholdMin = 10;
const int tresholdMax = 30;

//arrays to hold current and last state of buttons 
static byte lastState[numBut];
boolean currentState[numBut];
boolean wifiConnected = false;
static byte  rawState[numBut];


WiFiUDP Udp;                                
const IPAddress outIp(192, 168, 0, 100);     // La IP remota de la RPI

const unsigned int outPort = 7777;          // remote port to receive OSC
const unsigned int localPort = 7778;        // local port to listen for OSC packets (actually not used for sending)

// Variables will change:
int ledState = LOW;        // the current state of the output pin
int buttonState;            // the current reading from the input pin
int lastButtonState = LOW;  // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 80;   

void setup() {
  // set up serial port
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  Serial.begin(115200);
  
  //debug
  Serial.print("OSC sender with ");
  Serial.print(numBut, DEC);
  Serial.println(" buttons");
 
  //set up button pins as inputs 
  for (int i=0; i<numBut; i++){
    pinMode(touchButtons[i], INPUT);
    currentState[i] = false;
    Serial.print("Setup input ");
    Serial.println(i);
  }

  for (int i=0; i<numBut; i++){
    currentState[i] = false;
  }
}

void loop() {
  if(WiFi.status() != WL_CONNECTED) { wifiConnected = false; }
  checkTouch();
  debounce(buttonPin);
  if(wifiConnected == true) {
    digitalWrite(ledPin, ledState);
  }
  delay(10);
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
  wifiConnected = true;

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(localPort);
}

void checkTouch() {
  //int i = 4; //Prueba
  for (int i=0; i<numBut; i++){
    rawState[i] = touchRead(touchButtons[i]);
    if (rawState[i] <= tresholdMin) {
      currentState[i] = true;   
    } else if(rawState[i] >= tresholdMax) {  
      currentState[i] = false;
    }

    if(currentState[i] != lastState[i]) {
      //sendMessageRPI(i, rawState[i]);
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

void debounce (int buttonPin) {
   // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        ledState = !ledState;
      }
    }
  }

  // set the LED:
  //digitalWrite(ledPin, ledState);
  Serial.println("BUTTON PRESSED");

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
}
