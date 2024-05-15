#include <Servo.h>
#include <movingAvg.h> // 17 inches off, 25 inch diag

#define SERVO_PWM 9 // Servo motor: Signal pin
#define PROXIMITY_TRIG 3 // Proximity Sensor: Ultrasonic pulse sending pin
#define PROXIMITY_ECHO 2 // Proximity Sensor: Ultrasinic pulse recieving pin

Servo motor;
movingAvg proximitySensor(10);
const int LID_CLOSE_ANGLE = 57;
const int LID_OPEN_ANGLE = 0;
const float OPENING_THRESHOLD_CM = 80;
const float CLOSING_THRESHOLD_CM = 120;

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

// Rotate motor degree by degree to desiredAngle
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
    String state;
    float distanceCM = proximitySensor.reading(ultrasonicMeasureCM());

    // Open, close, or standby toilet lid 
    if (distanceCM > CLOSING_THRESHOLD_CM) {
        state = "Closing...";
        motor.write(LID_CLOSE_ANGLE);
    }
    else if (distanceCM < OPENING_THRESHOLD_CM) {
        state = "Opening...";
        motor.write(LID_OPEN_ANGLE);
        delay(1000); // optional additional delay to prevent false negatives
    }
    else {
        state = "Waiting...";
    }
    
    Serial.println("| State: "+state+" | Distance: "+distanceCM+" | Angle: "+motor.read()+" |");
    delay(100);
}
