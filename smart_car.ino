#include <Wire.h>
#include "Adafruit_TCS34725.h"

// === PINS ===
#define trig1 3
#define echo1 4
#define trig2 A2
#define echo2 A1

#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 10
#define ENA 6
#define ENB 11

#define L1 12
#define L2 13
#define interruptPin 2   // CHANGED: clearer name

// === CONFIG ===
const uint16_t LOWT = 20;
int speedVal = 150;
int dis1;
int dis2;

bool actionTriggered = false;
bool redStopped = false;
bool isDark = false;
volatile bool checkDarkFlag = false;

uint16_t r, g, b, c;

// Track movement direction
enum Movement {STOPPED, FORWARD, BACKWARD, LEFT, RIGHT};
Movement currentMove = STOPPED;

// === TIMING FOR RED CHECK ===
unsigned long lastColorCheck = 0;
const unsigned long colorCheckInterval = 100;  // Check red every 100ms

// === SENSOR OBJECT ===
Adafruit_TCS34725 tcs = Adafruit_TCS34725
(
  TCS34725_INTEGRATIONTIME_50MS,
  TCS34725_GAIN_4X
);

// === INTERRUPT HANDLER ===
void darkISR() 
{
  checkDarkFlag = true;
}

void checkRedColor()
{
  tcs.getRawData(&r, &g, &b, &c);
  // CHANGED: Normalize RGB to avoid false triggers from brightness
  if (c == 0) c = 1;
  // avoid division by zero
  float redRatio = (float)r / c;
  float greenRatio = (float)g / c;
  float blueRatio = (float)b / c;

  bool isRedDetected = (redRatio > 0.4) && (redRatio > greenRatio * 1.5) && (redRatio > blueRatio * 1.5);
  if (isRedDetected && !redStopped) 
  {
    stopmotors();
    redStopped = true;
  } 
  else if (!isRedDetected && redStopped) 
  {
    redStopped = false;
    // allow movement again when red is gone
  }
  delay (100);
}

void setup() {
  pinMode(trig1, OUTPUT);
  pinMode(echo1, INPUT);
  pinMode(trig2, OUTPUT);
  pinMode(echo2, INPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);
  pinMode(interruptPin, INPUT);

  analogWrite(ENA, speedVal);
  analogWrite(ENB, speedVal);

  Serial.begin(9600);  // For Bluetooth/Debug

  tcs.begin();
  // CHANGED: clear naming for interrupt
  attachInterrupt(digitalPinToInterrupt(interruptPin), darkISR, CHANGE);
}

void loop() 
{
  unsigned long currentMillis = millis();
  // === Periodic RED Detection ===
  if (currentMillis - lastColorCheck >= colorCheckInterval) 
  {
    lastColorCheck = currentMillis;
    checkRedColor();
  }

  // === DARKNESS DETECTION ===
  if (checkDarkFlag) 
  {
    checkDarkFlag = false;
    tcs.getRawData(&r, &g, &b, &c);
    isDark = (c < LOWT);
    if (isDark && !actionTriggered) 
    {
      digitalWrite(L1, HIGH);
      digitalWrite(L2, HIGH);
      actionTriggered = true;
    } 
    else if (!isDark && actionTriggered) 
    {
      digitalWrite(L1, LOW);
      digitalWrite(L2, LOW);
      actionTriggered = false;
    }
  }

  // === OBSTACLE DETECTION WITH DIRECTION AWARENESS ===
  // if ((currentMove == FORWARD && ObstacleCheck1()) ||
  //   (currentMove == BACKWARD && ObstacleCheck2())) 
  // {
  //   stopmotors();
  // }

  // === BLUETOOTH CONTROL ===
  if (Serial.available()) 
  {
    char command = Serial.read();
    switch (command) 
    {
      case 'f': forward(); break;
      case 'b': backward(); break;
      case 'l': left(); break;
      case 'r': right(); break;
      case 's': stopmotors(); break;
    }
  }
}

// === OBSTACLE CHECK FUNCTIONS ===
bool ObstacleCheck1() {
  dis1 = readDistance(trig1, echo1);
  return (dis1 <= 5);
}

bool ObstacleCheck2() {
  dis2 = readDistance(trig2, echo2);
  return (dis2 <= 5);
}

int readDistance(int trig, int echo) {
  long duration;
  int distance;
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH); // 20ms timeout (~3.4m)
  distance = duration * 0.034 / 2;
  return distance;
}

// === MOTION FUNCTIONS ===
void forward() 
{
  if (!ObstacleCheck1()) 
  {
    currentMove = FORWARD;
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENA, speedVal);
    analogWrite(ENB, speedVal);
  }    
}

void backward() 
{
  if (!ObstacleCheck2() && !redStopped) 
  {
    currentMove = BACKWARD;
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENA, speedVal);
    analogWrite(ENB, speedVal);
  }
  if ((currentMove == BACKWARD && ObstacleCheck2())) 
  {
    Serial.println(currentMove);
    Serial.println(readDistance(trig2,echo2));
    Serial.println(ObstacleCheck2());
    stopmotors();
    Serial.println(currentMove);
    Serial.println(readDistance(trig2,echo2));
    Serial.println(ObstacleCheck2());
  }
}

void left() 
{
  // if (!redStopped) 
  // {
  currentMove = LEFT;
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, speedVal*.9);
  analogWrite(ENB, speedVal*.9);
  delay(500);
  stopmotors();
  // }
}

void right() {
  // if (!redStopped) {
  currentMove = RIGHT;
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, speedVal);
  analogWrite(ENB, speedVal);
  delay(500);
  stopmotors();
  // }
}

void stopmotors() 
{
  currentMove = STOPPED;
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}
