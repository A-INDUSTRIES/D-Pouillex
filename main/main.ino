#include "Adafruit_TCS34725.h"
#include "Adafruit_Crickit.h"
#include "seesaw_servo.h"
#include "seesaw_motor.h"

#define NUM_SERVOS 4
#define NUM_MOTORS 2

#define BUTTON1 CRICKIT_SIGNAL1
#define BUTTON2 CRICKIT_SIGNAL2

Adafruit_Crickit crickit;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);

seesaw_Servo servos[4] = { seesaw_Servo(&crickit),
                           seesaw_Servo(&crickit),
                           seesaw_Servo(&crickit),
                           seesaw_Servo(&crickit) };

seesaw_Motor motors[2] = { seesaw_Motor(&crickit),
                           seesaw_Motor(&crickit) };

int servoPins[4] = { CRICKIT_SERVO1, CRICKIT_SERVO2, CRICKIT_SERVO3, CRICKIT_SERVO4 };
int motorPins[2][2] = { { CRICKIT_MOTOR_A1, CRICKIT_MOTOR_A2 }, { CRICKIT_MOTOR_B1, CRICKIT_MOTOR_B2 } };

void setup() {
  Serial.begin(115200);

  if (!crickit.begin()) {
    Serial.println("Error");
    while (1) delay(1);
  } else {
    Serial.println("Started!");
  }
  for (int i = 0; i < NUM_SERVOS; i++) {
    servos[i].attach(servoPins[i]);
  }
  for (int i = 0; i < NUM_MOTORS; i++) {
    motors[i].attach(motorPins[i][0], motorPins[i][1]);
  }
  crickit.pinMode(BUTTON1, INPUT_PULLUP);
  crickit.pinMode(BUTTON2, INPUT_PULLUP);
  tcs.begin();
}

void rotateServo(int servo, int angle) {
  servos[servo - 1].write(angle);
}

void runMotor(int motor, float speed) {
  motors[motor - 1].throttle(speed);
}

int getColor() {
  unit16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);
  return {&r, &g, &b, &c};
}

void loop() {
  Serial.println(getColor());
  delay(100);
}
