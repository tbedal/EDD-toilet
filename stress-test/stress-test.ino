#include <Servo.h>

#define SERVO_PWM 9

Servo motor;

void setup() {
    Serial.begin(9600);
    motor.attach(SERVO_PWM);
    Serial.println("Program initialized.");
}

// Rotate motor from current angle to angleStop in a set amount of time
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
    Serial.println("Starting test...");

    for (int i = 0; i < 10; i++) {
        Serial.print("Starting cycle #");
        Serial.print(i);
        Serial.print("... ");

        rotateMotor(0, 2000);
        delay(1500);
        rotateMotor(90, 2000);
        delay(1500);

        Serial.println("Complete!");

    Serial.print("Test complete!");

    delay(2000);
    exit(0);
}