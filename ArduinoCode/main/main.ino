// Basic libraries
#include <Adafruit_Crickit.h>
#include <seesaw_servo.h>
#include <seesaw_motor.h>

// Color sensor
#include <Adafruit_TCS34725.h>

// Ultrasound sensor
#include <DFRobot_URM09.h>

// OLED Screen
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define NUM_SERVOS 4
#define NUM_MOTORS 2
#define NUM_IR 4

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define IR_SENSITIVITY 200

#define OLED_RESET -1

#define SCREEN_ADDRESS 0x3C

#define R_Coef 0.136
#define G_Coef 1.000
#define B_Coef -0.444

Adafruit_Crickit crickit;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_16X);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DFRobot_URM09 URM09;

seesaw_Servo servos[4] = { seesaw_Servo(&crickit),
                           seesaw_Servo(&crickit),
                           seesaw_Servo(&crickit),
                           seesaw_Servo(&crickit) };

seesaw_Motor motor = seesaw_Motor(&crickit);

uint16_t ir_Sensor[4] = { CRICKIT_SIGNAL3, CRICKIT_SIGNAL4, CRICKIT_SIGNAL5, CRICKIT_SIGNAL6 };

int servoPins[4] = { CRICKIT_SERVO1, CRICKIT_SERVO2, CRICKIT_SERVO3, CRICKIT_SERVO4 };
int motorPins[2] = { CRICKIT_MOTOR_A1, CRICKIT_MOTOR_A2 };

int smallWhiteCount = 0;
int bigWhiteCount = 0;

int smallBlackCount = 0;
int bigBlackCount = 0;

int pass = 0;
int queued = 0;

struct Colors {
  uint16_t r;
  uint16_t g;
  uint16_t b;
  uint16_t c;
};

struct Colors colors(uint16_t r, uint16_t g, uint16_t b, uint16_t c) {
  struct Colors color;
  color.r = r;
  color.g = g;
  color.b = b;
  color.c = c;

  return color;
};

void setup() {
  if (!crickit.begin()) {
    while (1) delay(1);
  }

  for (int i = 0; i < NUM_SERVOS; i++) {
    servos[i].attach(servoPins[i]);
  }

  motor.attach(motorPins[0], motorPins[1]);

  for (int i = 0; i < NUM_IR; i++) {
    crickit.pinMode(ir_Sensor[i], INPUT_PULLUP);
  }
  tcs.begin();

  URM09.begin();
  URM09.setModeRange(MEASURE_MODE_PASSIVE, MEASURE_RANG_500);

  display.begin(SSD1306_SWITCHCAPVCC, 0X3C);

  // Delay of two seconds to let the display initialize.
  delay(2000);
  showReady();
}

void loop() {
  if (pass == 1000 && (smallBlackCount + bigBlackCount + smallWhiteCount + bigWhiteCount) != 0) {
    while (true) {
      showResults();
    }
  }
  if ((smallBlackCount + bigBlackCount + smallWhiteCount + bigWhiteCount) != 0) {
    pass += 1;
  }
  if (distance() < 3 || queued > 0) {
    showWorking();
    if (queued >= 1) {
      queued -= 1;
    }
    pass = 0;
    bool white = isWhite();
    if (white) {
      fallDown();
    } else {
      raiseUp();
    }
    if (distance() < 3) {
      queued += 1;
    }
    delay(100);
    pushBlock();
    delay(100);
    count();
    showReady();
  }
  delay(10);
}

void count() {
  // For a limited amount of time, waits and see if a vote passes in front of a sensor, if so count it.
  for (int i = 0; i < 500; i++) {
    if (readIr(0) > IR_SENSITIVITY) {
      bigWhiteCount += 1;
      break;
    }
    if (readIr(1) > IR_SENSITIVITY) {
      smallWhiteCount += 1;
      break;
    }
    if (readIr(2) > IR_SENSITIVITY) {
      bigBlackCount += 1;
      break;
    }
    if (readIr(3) > IR_SENSITIVITY) {
      smallBlackCount += 1;
      break;
    }
  }
}

int16_t distance() {
  URM09.measurement();
  int16_t dist = URM09.getDistance();
  return dist;
}

void drawText(int i) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 9 + i);
  display.println("P   B  :");
  display.setCursor(0, 26 + i);
  display.println("G   B  :");
  display.setCursor(0, 43 + i);
  display.println("P   N  :");
  display.setCursor(0, 60 + i);
  display.println("G   N  :");

  display.setCursor(100, 9 + i);
  display.println(smallWhiteCount);
  display.setCursor(100, 26 + i);
  display.println(bigWhiteCount);
  display.setCursor(100, 43 + i);
  display.println(smallBlackCount);
  display.setCursor(100, 60 + i);
  display.println(bigBlackCount);

  display.setTextSize(1);
  display.setCursor(12, 16 + i);
  display.println("etits   lanc");
  display.setCursor(12, 32 + i);
  display.println("rands   lanc");
  display.setCursor(12, 50 + i);
  display.println("etits   oir");
  display.setCursor(12, 67 + i);
  display.println("rands   oir");

  display.setTextSize(2);
  display.fillRect(0, 0, 128, 16, BLACK);
  display.setCursor(0, 0);
  display.println("Resultats:");

  display.display();
}

void fallDown() {
  rotateServo(1, 90);
}

struct Colors getColor() {
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);
  uint16_t ir = (r + g + b > c) ? (r + g + b - c) / 2 : 0;
  uint16_t r_comp = r - ir;
  uint16_t g_comp = g - ir;
  uint16_t b_comp = b - ir;
  struct Colors color;
  color = colors(r_comp, g_comp, b_comp, c);
  return color;
}

bool isWhite() {
  struct Colors color = getColor();
  if (color.r < 350 || color.g < 350 || color.b < 350) {
    return false;
  }
  return true;
}

void pushBlock() {
  runMotor(0.8);
  delay(210);
  runMotor(0);
  delay(10);
  runMotor(-0.8);
  delay(220);
  runMotor(0);
}

void raiseUp() {
  rotateServo(1, 0);
}

int16_t readIr(pin_size_t index) {
  int16_t value = crickit.analogRead(ir_Sensor[index]);
  return value;
}

void rotateServo(int servo, int angle) {
  servos[servo - 1].write(angle);
}

void runMotor(float speed) {
  motor.throttle(speed);
}

void showReady() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Pret.");
  display.setTextSize(1);
  display.setCursor(0, 30);
  display.println("Pour commencer,");
  display.setCursor(0, 40);
  display.println("inserez un vote.");
  display.display();
}

void showResults() {
  // Makes the result text scroll vertically in both directions.
  for (int i = -75; i < 65; i++) {
    drawText(i);
  }
  for (int i = 65; i > -75; i--) {
    drawText(i);
  }
}

void showWorking() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Occupe...");
  display.setTextSize(1);
  display.setCursor(0, 30);
  display.println("Vous pouvez continuer");
  display.setCursor(0, 40);
  display.println("a inserer les votes.");
  display.display();
}
