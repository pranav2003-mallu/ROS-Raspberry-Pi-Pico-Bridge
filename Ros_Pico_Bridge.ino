/*********************************************************************
 * ROS Pico Bridge
 * 
 * Raspberry Pi Pico + L298N driver.
 * Compatible with ROS 2 teleop or autonomous commands via serial/micro-ROS.
 *
 * Author: pranav s pillai
 * Email: pranavspillai2003@gmail.com
 * License: Open-source — free to modify/use with credit.
 * Note: u can modify this code as per your requirements.
 * this code is open-source and free to use.so if u find it useful,kindly give credit and support. if needed reach out to me.i will help you out.
 *********************************************************************/

#define USE_BASE
#define L298_MOTOR_DRIVER
#define ARDUINO_ENC_COUNTER
#define LEFT  0
#define RIGHT 1

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

// -------------------- LED PINS --------------------
#define LED_BUILTIN_PIN 25   // Pico onboard LED
#define LED_ROS_STATUS  16    // LED: ON when ROS connected
#define LED_MODE        17   // LED: Teleop/Auto mode indicator
// -------------------------------------------------

// Command parsing variables
int arg = 0;
int cmd_index = 0;
char chr;
char cmd;
char argv1[16];
char argv2[16];
long arg1;
long arg2;

// State variables
bool ros_connected = false;
bool teleop_mode = true;
unsigned long last_serial_rx = 0;
unsigned long last_led_blink = 0;
bool led_state = false;

void resetCommand() {
  cmd = 0;
  memset(argv1, 0, sizeof(argv1));
  memset(argv2, 0, sizeof(argv2));
  arg1 = 0;
  arg2 = 0;
  arg = 0;
  cmd_index = 0;
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
      teleop_mode = true;
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
      teleop_mode = false;  // Autonomous control
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

  // Initialize LEDs
  pinMode(LED_BUILTIN_PIN, OUTPUT);
  pinMode(LED_ROS_STATUS, OUTPUT);
  pinMode(LED_MODE, OUTPUT);
  digitalWrite(LED_BUILTIN_PIN, LOW);
  digitalWrite(LED_ROS_STATUS, LOW);
  digitalWrite(LED_MODE, LOW);
}

void loop() {
  // ---------- Serial Command Handling ----------
  while (Serial.available() > 0) {
    chr = Serial.read();
    ros_connected = true;
    last_serial_rx = millis();

    if (chr == 13) { // Enter key
      if (arg == 1) argv1[cmd_index] = 0;
      else if (arg == 2) argv2[cmd_index] = 0;
      runCommand();
      resetCommand();
    } else if (chr == ' ') {
      if (arg == 0) arg = 1;
      else if (arg == 1) {
        argv1[cmd_index] = 0;
        arg = 2;
        cmd_index = 0;
      }
    } else {
      if (arg == 0) cmd = chr;
      else if (arg == 1) argv1[cmd_index++] = chr;
      else if (arg == 2) argv2[cmd_index++] = chr;
    }
  }

  // ---------- PID Update ----------
  if (millis() > nextPID) {
    updatePID();
    nextPID += PID_INTERVAL;
  }

  // ---------- Auto-stop ----------
  if ((millis() - lastMotorCommand) > AUTO_STOP_INTERVAL) {
    setMotorSpeeds(0, 0);
    moving = 0;
  }

  // ---------- LED Behavior ----------
  // Blink built-in LED (heartbeat)
  if (millis() - last_led_blink > 500) {
    led_state = !led_state;
    digitalWrite(LED_BUILTIN_PIN, led_state);
    last_led_blink = millis();
  }

  // ROS connection LED (turn off if no data for 2s)
  if (ros_connected && (millis() - last_serial_rx < 2000)) {
    digitalWrite(LED_ROS_STATUS, HIGH);
  } else {
    digitalWrite(LED_ROS_STATUS, LOW);
    ros_connected = false;
  }

  // Mode LED
  if (teleop_mode)
    digitalWrite(LED_MODE, HIGH);   // Teleop mode = ON
  else
    digitalWrite(LED_MODE, LOW);    // Autonomous mode = OFF
}

