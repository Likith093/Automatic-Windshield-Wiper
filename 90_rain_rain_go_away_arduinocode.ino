#include <Servo.h>

// Pins
#define ENCODER_PIN_A 2  // Encoder pin A (attach interrupt)
#define ENCODER_PIN_B 3  // Encoder pin B (optional, for direction)
#define SERVO_PIN 9      // Servo control pin
#define outputPin 13
int state = 0;
// Variables
volatile int pulseCount = 0; // Encoder pulse count
const int PPR = 100;         // Pulses per revolution of encoder
Servo speedometerServo;      // Servo object
void setup() {
    pinMode(outputPin, OUTPUT);
    digitalWrite(outputPin, LOW);
    Serial.begin(9600);


     // Initialize encoder pin
  pinMode(ENCODER_PIN_A, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), countPulses, RISING);

  // Initialize servo
  speedometerServo.attach(SERVO_PIN);
  speedometerServo.write(0); // Start at 0°

  Serial.begin(9600); // Debugging
}

void loop() {
    if(Serial.available() > 0){ // Checks whether data is coming from the serial port
        state = Serial.read(); // Reads the data from the serial port
    }

    if (state == '0') {
        digitalWrite(outputPin, LOW); // Turn LED OFF
        state = 0;
    }
    else if (state == '1') {
        digitalWrite(outputPin, HIGH);
        state = 0;
    }



    // Calculate speed
  int rpm = calculateRPM();
  
  // Determine speed level
  int speedLevel = getSpeedLevel(rpm);

  // Map speed level to servo position
  int servoAngle = map(speedLevel, 0, 3, 0, 180);
  speedometerServo.write(servoAngle);

  // Debugging
  Serial.print("RPM: ");
  Serial.print(rpm);
  Serial.print(", Speed Level: ");
  Serial.print(speedLevel);
  Serial.print(", Servo Angle: ");
  Serial.println(servoAngle);

  delay(500); // Update every 500 ms
 }

// Interrupt Service Routine (ISR)
void countPulses() {
  pulseCount++;
}

// Calculate RPM
int calculateRPM() {
  static unsigned long lastTime = 0;
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - lastTime;

  int pulses = pulseCount;
  pulseCount = 0; // Reset count for the next interval

  lastTime = currentTime;
  return (pulses * 60000L) / (PPR * elapsedTime);
}

// Determine speed level based on RPM
int getSpeedLevel(int rpm) {
  if (rpm == 0) return 0;        // Stopped
  else if (rpm < 50) return 1; // Low speed
  else if (rpm < 80) return 2; // Medium speed
  else return 3;                 // High speed
}