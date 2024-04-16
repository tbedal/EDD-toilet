#define MOTOR_POSITIVE 8 // DC Motor: Positive pin
#define MOTOR_NEGATIVE 7 // DC Motor: Negative pin
#define PROXIMITY_TRIG 3 // Proximity Sensor: Ultrasonic pulse sending pin
#define PROXIMITY_ECHO 2 // Proximity Sensor: Ultrasinic pulse recieving pin
#define POTENTIOMETER A5 // Potentiometer: Analog input pin

const int NUM_SAMPLES = 20;
const int NUM_OUTLIERS = 5;
const float ACTIVIATION_THRESHOLD_CM = 10.00;
int cyclesUserAbsent;

// Get data from ultrasonic sensor and convert to distance in centimeters
float ultrasonicMeasureCM() {
  digitalWrite(PROXIMITY_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PROXIMITY_TRIG, LOW);

  return 0.017 * pulseIn(PROXIMITY_ECHO, HIGH); // Conversion to CM
}

// Spin motor clockwise
void rotateRight() {
  digitalWrite(MOTOR_NEGATIVE, LOW);
  digitalWrite(MOTOR_POSITIVE, HIGH);
}

// Spin motor counter-clockwise
void rotateLeft() {
  digitalWrite(MOTOR_NEGATIVE, HIGH);
  digitalWrite(MOTOR_POSITIVE, LOW);
}

// Cut power to motor
void stopRotation() {
  digitalWrite(MOTOR_NEGATIVE, LOW);
  digitalWrite(MOTOR_POSITIVE, LOW);
}

void setup() {
  Serial.begin(9600);
  pinMode(PROXIMITY_TRIG, OUTPUT);
  pinMode(PROXIMITY_ECHO, INPUT);
  pinMode(POTENTIOMETER, INPUT);
  pinMode(MOTOR_POSITIVE, OUTPUT);
  pinMode(MOTOR_NEGATIVE, OUTPUT);
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
  int potAngleDegrees = analogRead(POTENTIOMETER);

  // TODO: this is inefficient. Switch to a rolling average?
  // Keep track of how long user has been absent
  cyclesUserAbsent = distanceCM < ACTIVIATION_THRESHOLD_CM ? 0 : cyclesUserAbsent + 1;
  if (cyclesUserAbsent >= 1000) {
    cyclesUserAbsent = 3; //make sure cycle count does not reach integer limit
  }

  // Open, close, or standby toilet lid 
  if (cyclesUserAbsent > 3 && potAngleDegrees > 500) {
    rotateLeft();
    Serial.println("Closing the lid!");
  }
  else if (cyclesUserAbsent == 0 && potAngleDegrees < 5) {
    rotateRight();
    Serial.println("Opening the lid!");
  }
  else {
    stopRotation();
    Serial.println("Waiting for movement...");
  }

  // Serial monitor readout
  Serial.print(distanceCM);
  Serial.print("  ");
  Serial.print(potAngleDegrees);
  Serial.print("  ");
  Serial.println(cyclesUserAbsent);
}