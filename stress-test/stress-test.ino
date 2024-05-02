#include <Servo.h>

#define SERVO_PWM 9

Servo motor;

void setup() {
    Serial.begin(9600);
    motor.attach(SERVO_PWM);
    Serial.println(motor.read());
    Serial.println("Program initialized.");
}

// Rotate motor from current angle to angleStop in a set amount of time
void rotateMotor(int angleStop, unsigned long desiredMovingTimeMS) {
    int angleStart = motor.read();
    unsigned long elapsedMovingTimeMS = millis();

    while (motor.read() != angleStop) {
        unsigned long progress = millis() - elapsedMovingTimeMS;
        long angle = map(progress, 0, desiredMovingTimeMS, angleStart, angleStop); // I don't know why this works
        motor.write(angle);
    }
}

void rotateMotorBruteForce(int desiredAngle) {
    int startAngle = motor.read();
    int increment = desiredAngle > startAngle ? 1 : -1;
    
    for (int angle = startAngle; angle != desiredAngle; angle += increment) {
        Serial.println(angle);
        motor.write(angle);
        delay(20);
    }
}

// Run close at 10; Open at 48
void runCycle(int angle1, int angle2, int delayMilliseconds) {
    Serial.println("Closing!");
    motor.write(angle1);
    delay(delayMilliseconds);
    Serial.println("Opening!");
    motor.write(angle2);
    delay(delayMilliseconds);
}

void writeMotorManually() {
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        int angle = command.toInt();
        motor.write(angle);
    }
}

int cycles = 0;
void loop() {
    runCycle(50, 5, 2000);
    cycles++;
    Serial.println(cycles);
}
