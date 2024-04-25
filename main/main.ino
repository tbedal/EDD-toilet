#include <Servo.h>
#include <movingAvg.h>

#define SERVO_PWM 9 // Servo motor: Signal pin
#define PROXIMITY_TRIG 3 // Proximity Sensor: Ultrasonic pulse sending pin
#define PROXIMITY_ECHO 2 // Proximity Sensor: Ultrasinic pulse recieving pin

Servo motor;
movingAvg proximitySensor(20);
const float ACTIVIATION_THRESHOLD_CM = 10.00;

void setup() {
    Serial.begin(9600);
    pinMode(PROXIMITY_TRIG, OUTPUT);
    pinMode(PROXIMITY_ECHO, INPUT);
    motor.attach(SERVO_PWM);
    proximitySensor.begin();

    Serial.println("Program Initialized.");
}

// Get data from ultrasonic sensor and convert to distance in centimeters
float ultrasonicMeasureCM() {
    digitalWrite(PROXIMITY_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(PROXIMITY_TRIG, LOW);

    return 0.017 * pulseIn(PROXIMITY_ECHO, HIGH); // Conversion to CM
}

void rotateMotor(int angleStop, unsigned long desiredMovingTimeMS) {
    int angleStart = motor.read();
    unsigned long elapsedMovingTimeMS = millis();

    while (motor.read() != angleStop) {
        unsigned long progress = millis() - elapsedMovingTimeMS;
        long angle = map(progress, 0, desiredMovingTimeMS, angleStart, angleStop); // I don't know why this works
        motor.write(angle);
    }
}

void loop() {
    // Take measurements
    float distanceCM = proximitySensor.reading(ultrasonicMeasureCM());
    int servoAngle = motor.read();

    // Open, close, or standby toilet lid 
    if (distanceCM > ACTIVIATION_THRESHOLD_CM && servoAngle > 5) {
        Serial.println("Closing the lid!");
        rotateMotor(0, 5000); // opens new loop
    }
    else if (distanceCM >= ACTIVIATION_THRESHOLD_CM && servoAngle <= 5) {
        Serial.println("Opening the lid!");
        rotateMotor(90, 5000); // opens new loop
    }
    else {
        Serial.println("Waiting for movement...");
        // Serial monitor readout
        Serial.print(distanceCM);
        Serial.print("    ");
        Serial.println(servoAngle);
    }
}