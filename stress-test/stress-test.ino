#define MOTOR_POSITIVE 17 // DC Motor: Positive pin
#define MOTOR_NEGATIVE 16 // DC Motor: Negative pin
#define POTENTIOMETER A5 // Potentiometer: Analog input pin

const int HISTORY_SIZE = 5;
int angles[HISTORY_SIZE];

void setup() {
    Serial.begin(9600);
    pinMode(POTENTIOMETER, INPUT);
    pinMode(MOTOR_POSITIVE, OUTPUT);
    pinMode(MOTOR_NEGATIVE, OUTPUT);
    for (int i = 0; i < HISTORY_SIZE; i++) {
        angles[i] = analogRead(POTENTIOMETER);
    }
}

void addNewAngle(int newAngle) {
    for (int i = 0; i < HISTORY_SIZE-1; i++) {
        angles[i] = angles[i+1];
    }
    angles[HISTORY_SIZE-1] = newAngle;
}

// TODO: if current potentiometer reading is the same as the average of the last 5, then sound alarm
int getAvgAngle() {
    int runningTotal = 0;
    for (int angle : angles) {
        runningTotal += angle;
    }
    return runningTotal / HISTORY_SIZE;
}

void openLid() {
    while (analogRead(POTENTIOMETER) < 200) {
        digitalWrite(MOTOR_POSITIVE, LOW);
        digitalWrite(MOTOR_NEGATIVE, HIGH);
    }
}

void closeLid() {
    while (analogRead(POTENTIOMETER) > 0) {
        digitalWrite(MOTOR_NEGATIVE, LOW);
        digitalWrite(MOTOR_POSITIVE, HIGH);
    }
}

void loop() {
    for (int i = 0; i < 100; i++) {
        openLid();
        delay(1000);
        closeLid();
    }
}