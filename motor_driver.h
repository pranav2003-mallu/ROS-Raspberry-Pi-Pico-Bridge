#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#define LEFT_MOTOR_EN  9
#define LEFT_MOTOR_IN1 10
#define LEFT_MOTOR_IN2 11

#define RIGHT_MOTOR_EN 6
#define RIGHT_MOTOR_IN1 7
#define RIGHT_MOTOR_IN2 8

void initMotorController() {
  pinMode(LEFT_MOTOR_EN, OUTPUT);
  pinMode(LEFT_MOTOR_IN1, OUTPUT);
  pinMode(LEFT_MOTOR_IN2, OUTPUT);
  pinMode(RIGHT_MOTOR_EN, OUTPUT);
  pinMode(RIGHT_MOTOR_IN1, OUTPUT);
  pinMode(RIGHT_MOTOR_IN2, OUTPUT);
}

void setMotorSpeeds(int leftSpeed, int rightSpeed) {
  leftSpeed = constrain(leftSpeed, -MAX_PWM, MAX_PWM);
  rightSpeed = constrain(rightSpeed, -MAX_PWM, MAX_PWM);

  // Left motor
  if (leftSpeed >= 0) {
    digitalWrite(LEFT_MOTOR_IN1, HIGH);
    digitalWrite(LEFT_MOTOR_IN2, LOW);
  } else {
    digitalWrite(LEFT_MOTOR_IN1, LOW);
    digitalWrite(LEFT_MOTOR_IN2, HIGH);
    leftSpeed = -leftSpeed;
  }

  // Right motor
  if (rightSpeed >= 0) {
    digitalWrite(RIGHT_MOTOR_IN1, HIGH);
    digitalWrite(RIGHT_MOTOR_IN2, LOW);
  } else {
    digitalWrite(RIGHT_MOTOR_IN1, LOW);
    digitalWrite(RIGHT_MOTOR_IN2, HIGH);
    rightSpeed = -rightSpeed;
  }

  analogWrite(LEFT_MOTOR_EN, leftSpeed);
  analogWrite(RIGHT_MOTOR_EN, rightSpeed);
}

#endif
