#define TRIG_PIN 12 // TRIG pin
#define ECHO_PIN 13 // ECHO pin
#define TRIG_PIN2 26 // TRIG pin
#define ECHO_PIN2 27 // ECHO pin

int filterWindow = 20;
float filterArray[20]; // array to store data samples from sensor
float filterArray2[20]; // array to store data samples from sensor
float distance; // store the distance from sensor
float distance2; // store the distance from sensor

void setup() {
  // begin serial port
  Serial.begin (115200);

  // configure the trigger and echo pins to output mode
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN2, OUTPUT);
  pinMode(ECHO_PIN2, INPUT);
}

void loop() {
  // 1. TAKING MULTIPLE MEASUREMENTS AND STORE IN AN ARRAY
  for (int sample = 0; sample < filterWindow; sample++) {
    filterArray[sample] = ultrasonicMeasure();
    filterArray2[sample] = ultrasonicMeasure2();
    delay(5); // to avoid untrasonic interfering
  }

  // 2. SORTING THE ARRAY IN ASCENDING ORDER
  for (int i = 0; i < 19; i++) {
    for (int j = i + 1; j < 20; j++) {
      if (filterArray[i] > filterArray[j]) {
        float swap = filterArray[i];
        filterArray[i] = filterArray[j];
        filterArray[j] = swap;
      }
      if (filterArray2[i] > filterArray2[j]) {
        float swap2 = filterArray2[i];
        filterArray2[i] = filterArray2[j];
        filterArray2[j] = swap2;
      }
    }
  }

  // 3. FILTERING NOISE
  // + the five smallest samples are considered as noise -> ignore it
  // + the five biggest  samples are considered as noise -> ignore it
  // ----------------------------------------------------------------
  // => get average of the 10 middle samples (from 5th to 14th)
  double sum = 0;
  double sum2 = 0;
  for (int sample = 5; sample < 15; sample++) {
    sum += filterArray[sample];
    sum2 += filterArray2[sample];
  }

  distance = sum / 10;
  distance2 = sum2 / 10;

  // print the value to Serial Monitor
  Serial.print("Distancia 1: ");
  Serial.print(distance);
  Serial.println(" cm");
  Serial.print("Distancia 2: ");
  Serial.print(distance2);
  Serial.println(" cm");
}

float ultrasonicMeasure() {
  // generate 10-microsecond pulse to TRIG pin
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // measure duration of pulse from ECHO pin
  float duration_us = pulseIn(ECHO_PIN, HIGH);

  // calculate the distance
  float distance_cm = 0.017 * duration_us;

  return distance_cm;
}

float ultrasonicMeasure2() {
  // generate 10-microsecond pulse to TRIG pin
  digitalWrite(TRIG_PIN2, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN2, LOW);

  // measure duration of pulse from ECHO pin
  float duration_us = pulseIn(ECHO_PIN2, HIGH);

  // calculate the distance
  float distance_cm = 0.017 * duration_us;

  return distance_cm;
}
