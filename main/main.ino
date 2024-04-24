#include <Servo.h>

#define SERVO_PWM 9 // Servo motor: Signal pin
#define PROXIMITY_TRIG 3 // Proximity Sensor: Ultrasonic pulse sending pin
#define PROXIMITY_ECHO 2 // Proximity Sensor: Ultrasinic pulse recieving pin

Servo motor;
const int NUM_SAMPLES = 20;
const int NUM_OUTLIERS = 5;
const float ACTIVIATION_THRESHOLD_CM = 10.00;
int cyclesUserAbsent;

void setup() {
  Serial.begin(9600);
  pinMode(PROXIMITY_TRIG, OUTPUT);
  pinMode(PROXIMITY_ECHO, INPUT);
  motor.attach(SERVO_PWM);

  Serial.println("Program Initialized.");
}

// Get data from ultrasonic sensor and convert to distance in centimeters
float ultrasonicMeasureCM() {
  digitalWrite(PROXIMITY_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PROXIMITY_TRIG, LOW);

  return 0.017 * pulseIn(PROXIMITY_ECHO, HIGH); // Conversion to CM
}

void rotateMotor(int angleStop, unsigned long movingTimeMs) {
  int angleStart = motor.read();
  unsigned long moveStartMs = millis();

  while (motor.read() != angleStop) {
    unsigned long progress = millis() - moveStartMs;
    long angle = map(progress, 0, movingTimeMs, angleStart, angleStop); // I don't know why this works
    motor.write(angle);
  }
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

  // TODO: this is inefficient. Switch to a rolling average?
  // Keep track of how long user has been absent
  cyclesUserAbsent = distanceCM < ACTIVIATION_THRESHOLD_CM ? 0 : cyclesUserAbsent + 1;
  if (cyclesUserAbsent >= 1000) {
    cyclesUserAbsent = 3; //make sure cycle count does not reach integer limit
  }

  // Open, close, or standby toilet lid 
  if (cyclesUserAbsent > 3 && motor.read() > 5) {
    Serial.println("Closing the lid!");
    rotateMotor(0, 5000);
  }
  else if (cyclesUserAbsent == 0 && motor.read() <= 5) {
    Serial.println("Opening the lid!");
    rotateMotor(97, 5000);
  }
  else {
    Serial.println("Waiting for movement...");
    // Serial monitor readout
    Serial.print(distanceCM);
    Serial.print("  ");
    Serial.print(motor.read());
    Serial.print("  ");
    Serial.println(cyclesUserAbsent);
  }
}