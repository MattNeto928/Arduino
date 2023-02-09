#include Servo.h

Servo servo;
const servoPin = 3;

void setup() {
  servo.attach(servoPin);
  servo.write(180);

}

void loop() {
  // put your main code here, to run repeatedly:

}
