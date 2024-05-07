#include <Servo.h>
#include <movingAvg.h> // 17 inches off, 25 inch diag

#define SERVO_PWM 9 // Servo motor: Signal pin
#define PROXIMITY_TRIG 3 // Proximity Sensor: Ultrasonic pulse sending pin
#define PROXIMITY_ECHO 2 // Proximity Sensor: Ultrasinic pulse recieving pin

Servo motor;
movingAvg proximitySensor(15);
const int LID_CLOSE_ANGLE = 60;
const int LID_OPEN_ANGLE = 0;
const float OPENING_THRESHOLD_CM = 80;
const float CLOSING_THRESHOLD_CM = 110;

void setup() {
    Serial.begin(9600);
    pinMode(PROXIMITY_TRIG, OUTPUT);
    pinMode(PROXIMITY_ECHO, INPUT);
    motor.attach(SERVO_PWM);
    motor.write(LID_CLOSE_ANGLE);
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

// Rotate motor to desired angleStop in desiredMovingTimeMS
void rotateMotorTimed(int angleStop, unsigned long desiredMovingTimeMS) {
    int angleStart = motor.read();
    unsigned long elapsedMovingTimeMS = millis();

    while (motor.read() != angleStop) {
        unsigned long progress = millis() - elapsedMovingTimeMS;
        long angle = map(progress, 0, desiredMovingTimeMS, angleStart, angleStop); // I don't know why this works
        motor.write(angle);
    }
}

void rotateMotor(int desiredAngle) {
    int startAngle = motor.read();
    int increment = desiredAngle > startAngle ? 1 : -1;
    
    for (int angle = startAngle; angle != desiredAngle; angle += increment) {
        Serial.println(angle);
        motor.write(angle);
        delay(20);
    }
}

void loop() {
    Serial.print("| State: ");

    // Take measurements
    int servoAngle = motor.read();
    float distanceCM = proximitySensor.reading(ultrasonicMeasureCM());

    // Open, close, or standby toilet lid 
    if (distanceCM > CLOSING_THRESHOLD_CM) {
        Serial.print("Closing...");
        motor.write(LID_CLOSE_ANGLE);
    }
    else if (distanceCM < OPENING_THRESHOLD_CM) {
        Serial.print("Opening...");
        motor.write(LID_OPEN_ANGLE);
    }
    else {
        Serial.print("Waiting...");
    }
    
    Serial.print(" | Distance: ");
    Serial.print(distanceCM);
    Serial.print(" | Angle: ");
    Serial.print(servoAngle);
    Serial.println(" |");

    delay(100);
}
