#ifndef DIFF_CONTROLLER_H
#define DIFF_CONTROLLER_H

struct PID {
  long TargetTicksPerFrame;
  float Encoder;
  float PrevError;
  float Output;
};

PID leftPID, rightPID;

float Kp = 20, Kd = 10, Ki = 0, Ko = 50;
bool moving = false;

void resetPID() {
  leftPID.PrevError = 0;
  rightPID.PrevError = 0;
}

void updatePID() {
  if (!moving) return;

  float leftError = leftPID.TargetTicksPerFrame - leftPID.Encoder;
  float rightError = rightPID.TargetTicksPerFrame - rightPID.Encoder;

  leftPID.Output += (Kp * leftError) + (Kd * (leftError - leftPID.PrevError));
  rightPID.Output += (Kp * rightError) + (Kd * (rightError - rightPID.PrevError));

  leftPID.PrevError = leftError;
  rightPID.PrevError = rightError;

  setMotorSpeeds(leftPID.Output, rightPID.Output);
}

#endif
