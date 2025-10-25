#ifndef ENCODER_DRIVER_H
#define ENCODER_DRIVER_H

#define LEFT_ENC_A 2
#define LEFT_ENC_B 3
#define RIGHT_ENC_A 4
#define RIGHT_ENC_B 5

volatile long leftTicks = 0;
volatile long rightTicks = 0;

void leftISR() {
  int b = digitalRead(LEFT_ENC_B);
  if (b > 0) leftTicks++;
  else leftTicks--;
}

void rightISR() {
  int b = digitalRead(RIGHT_ENC_B);
  if (b > 0) rightTicks++;
  else rightTicks--;
}

void initEncoders() {
  pinMode(LEFT_ENC_A, INPUT_PULLUP);
  pinMode(LEFT_ENC_B, INPUT_PULLUP);
  pinMode(RIGHT_ENC_A, INPUT_PULLUP);
  pinMode(RIGHT_ENC_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(LEFT_ENC_A), leftISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RIGHT_ENC_A), rightISR, CHANGE);
}

long readEncoder(int i) {
  return (i == 0) ? leftTicks : rightTicks;
}

void resetEncoders() {
  leftTicks = 0;
  rightTicks = 0;
}

#endif
