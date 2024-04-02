#define TRIG_PIN 3 // Proximity Sensor: Ultrasonic pulse sending pin
#define ECHO_PIN 2 // Proximity Sensor: Ultrasinic pulse recieving pin
#define POT_PIN A5 // Potentiometer: Analog input pin

const int NUM_SAMPLES = 20;
const int NUM_OUTLIERS = 5;
const float ACTIVIATION_THRESHOLD_CM = 10.00;
int cyclesUserAbsent;

// Get data from ultrasonic sensor and convert to distance in centimeters
float ultrasonicMeasureCM() {
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  return 0.017 * pulseIn(ECHO_PIN, HIGH); // Conversion to CM
}

void moveLid() {
  Serial.println("foobar!");
}

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(POT_PIN, INPUT);
}

void loop() {
  // Take multiple measurements and store in array
  float measurementsCM[NUM_SAMPLES];
  for (int sample = 0; sample < NUM_SAMPLES; sample++) {
    measurementsCM[sample] = ultrasonicMeasureCM();
    delay(30); // to avoid ultrasonic interference
  }

  // Sort measurements in ascending order
  for (int i = 0; i < NUM_SAMPLES-1; i++) {
    for (int j = i+1; j < NUM_SAMPLES; j++) {
      if (measurementsCM[i] > measurementsCM[j]) {
        measurementsCM[i] = measurementsCM[j];
        measurementsCM[j] = measurementsCM[i];
      }
    }
  }

  // Get sum of middle values
  double sum = 0;
  for (int sample = NUM_OUTLIERS; sample < (NUM_SAMPLES - NUM_OUTLIERS); sample++) {
    sum += measurementsCM[sample];
  }

  // Get average of middle samples
  float distanceCM = sum / (NUM_SAMPLES - (NUM_OUTLIERS * 2));
  int potAngleDegrees = analogRead(POT_PIN);

  // Keep track of how long user has been absent
  cyclesUserAbsent = distanceCM < ACTIVIATION_THRESHOLD_CM ? 0 : cyclesUserAbsent + 1;
  if (cyclesUserAbsent >= 500) {
    cyclesUserAbsent = 1; //make sure cycle count does not reach integer limit
  }

  // Open, close, or standby toilet lid 
  if (cyclesUserAbsent > 3 && potAngleDegrees > 500) {
    Serial.println("Closing the lid!");
  }
  else if (cyclesUserAbsent == 0 && potAngleDegrees < 5) {
    Serial.println("Opening the lid!");
  }
  else {
    Serial.println("Waiting for movement...");
  }

  // Serial monitor readout
  Serial.print(distanceCM);
  Serial.print("  ");
  Serial.println(potAngleDegrees);
}