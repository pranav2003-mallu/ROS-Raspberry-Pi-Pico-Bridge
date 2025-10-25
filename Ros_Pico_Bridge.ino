/*********************************************************************
 * ROS Pico Bridge
 * 
 * Raspberry Pi Pico + L298N driver.
 * Compatible with ROS 2 teleop or autonomous commands via serial/micro-ROS.
 *
 * Author: pranav s pillai
 * email: pranavspillai2003@gmail.com
 * Note: u can modify this code as per your requirements.
 * this code is open-source and free to use.so if u find it useful,kindly give credit and support. if needed reach out to me.i will help you out.
 *********************************************************************/

#define USE_BASE
#define L298_MOTOR_DRIVER
#define ARDUINO_ENC_COUNTER

#define BAUDRATE     115200
#define MAX_PWM      255

#include "Arduino.h"
#include "commands.h"
#include "motor_driver.h"
#include "encoder_driver.h"
#include "diff_controller.h"
#include "sensors.h"

// PID frequency (Hz)
#define PID_RATE 30
const int PID_INTERVAL = 1000 / PID_RATE;
unsigned long nextPID = PID_INTERVAL;

// Auto-stop after inactivity
#define AUTO_STOP_INTERVAL 2000
long lastMotorCommand = AUTO_STOP_INTERVAL;

// Command parsing variables
int arg = 0;
int index = 0;
char chr;
char cmd;
char argv1[16];
char argv2[16];
long arg1;
long arg2;

void resetCommand() {
  cmd = 0;
  memset(argv1, 0, sizeof(argv1));
  memset(argv2, 0, sizeof(argv2));
  arg1 = 0;
  arg2 = 0;
  arg = 0;
  index = 0;
}

int runCommand() {
  int i = 0;
  char *p = argv1;
  char *str;
  int pid_args[4];
  arg1 = atoi(argv1);
  arg2 = atoi(argv2);

  switch (cmd) {
    case GET_BAUDRATE:
      Serial.println(BAUDRATE);
      break;

    case ANALOG_READ:
      Serial.println(analogRead(arg1));
      break;

    case DIGITAL_READ:
      Serial.println(digitalRead(arg1));
      break;

    case ANALOG_WRITE:
      analogWrite(arg1, arg2);
      Serial.println("OK");
      break;

    case DIGITAL_WRITE:
      digitalWrite(arg1, arg2 ? HIGH : LOW);
      Serial.println("OK");
      break;

    case PIN_MODE:
      pinMode(arg1, arg2 ? OUTPUT : INPUT);
      Serial.println("OK");
      break;

    case READ_ENCODERS:
      Serial.print(readEncoder(LEFT));
      Serial.print(" ");
      Serial.println(readEncoder(RIGHT));
      break;

    case RESET_ENCODERS:
      resetEncoders();
      resetPID();
      Serial.println("OK");
      break;

    case MOTOR_SPEEDS:
      lastMotorCommand = millis();
      if (arg1 == 0 && arg2 == 0) {
        setMotorSpeeds(0, 0);
        resetPID();
        moving = 0;
      } else {
        moving = 1;
        leftPID.TargetTicksPerFrame = arg1;
        rightPID.TargetTicksPerFrame = arg2;
      }
      Serial.println("OK");
      break;

    case MOTOR_RAW_PWM:
      lastMotorCommand = millis();
      resetPID();
      moving = 0;
      setMotorSpeeds(arg1, arg2);
      Serial.println("OK");
      break;

    case UPDATE_PID:
      while ((str = strtok_r(p, ":", &p)) != NULL) {
        pid_args[i++] = atoi(str);
      }
      Kp = pid_args[0];
      Kd = pid_args[1];
      Ki = pid_args[2];
      Ko = pid_args[3];
      Serial.println("OK");
      break;

    default:
      Serial.println("Invalid Command");
      break;
  }
  return 1;
}

void setup() {
  Serial.begin(BAUDRATE);
  initMotorController();
  resetPID();
  initEncoders();
}

void loop() {
  while (Serial.available() > 0) {
    chr = Serial.read();

    if (chr == 13) { // Enter key
      if (arg == 1) argv1[index] = 0;
      else if (arg == 2) argv2[index] = 0;
      runCommand();
      resetCommand();
    } else if (chr == ' ') {
      if (arg == 0) arg = 1;
      else if (arg == 1) {
        argv1[index] = 0;
        arg = 2;
        index = 0;
      }
    } else {
      if (arg == 0) cmd = chr;
      else if (arg == 1) argv1[index++] = chr;
      else if (arg == 2) argv2[index++] = chr;
    }
  }

  if (millis() > nextPID) {
    updatePID();
    nextPID += PID_INTERVAL;
  }

  if ((millis() - lastMotorCommand) > AUTO_STOP_INTERVAL) {
    setMotorSpeeds(0, 0);
    moving = 0;
  }
}
