//Mux control pins
int s0 = 16;
int s1 = 17;
int s2 = 5;

//Mux salida comun 
int Z_pin = 34;

int muxChannel[8][3]={
    {0,0,0}, //channel 0
    {1,0,0}, //channel 1
    {0,1,0}, //channel 2
    {1,1,0}, //channel 3
    {0,0,1}, //channel 4
    {1,0,1}, //channel 5
    {0,1,1}, //channel 6
    {1,1,1}, //channel 7
};


void setup(){
  setMux();
  Serial.begin(115200);
}


void loop(){

  //Loop through and read all 8 values
  for(int i = 0; i < 8; i ++){
    Serial.print("Value at channel ");
    Serial.print(i);
    Serial.print(" is : ");
    Serial.println(readMux(i));
    delay(100);
  }

}

void setMux(){
  pinMode(s0, OUTPUT); 
  pinMode(s1, OUTPUT); 
  pinMode(s2, OUTPUT); 

  digitalWrite(s0, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s2, LOW);
}

float readMux(int channel){
  int controlPin[] = {s0, s1, s2};
  //loop through the 3 sig
  for(int i = 0; i < 3; i ++){
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }
  //read the value at the Z pin
  int val = analogRead(Z_pin);
  return val;
}
