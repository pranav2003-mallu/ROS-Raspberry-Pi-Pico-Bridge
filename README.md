# ROS Raspberry Pi Pico Bridge

A complete ROS (Robot Operating System) bridge for Raspberry Pi Pico with L298N motor driver, designed for differential drive robots. This package enables seamless communication between ROS and your Pico-based robot for both teleoperation and autonomous navigation.

# Features

* Differential drive control with PID velocity control.
* ROS serial communication via standard serial protocol.
* Encoder support for odometry calculation.
* L298N motor driver compatibility.
* Configurable PID parameters.
* Auto-stop safety feature.
* Sensor integration support.("According to your perpose and use").

# Hardware Requirements

   ^ Raspberry Pi Pico/Pico W .

   ^ L298N Motor Driver Module .
 
   ^ 2x 12V 240RPM L-shape Gear Motors with Encoders .

   ^ 12V Battery/Power Supply .

   ^ Motor Encoders (Quadrature) .

   ^ Jumper Wires and Breadboard/PCB .


   # Wire Connection Pico to Motor Driver.


     1️⃣ Confirm L298N Wiring

Here’s the correct connection for Pico + L298N (4-wheel car) setup:

L298N Pin	Connect To	Notes
IN1	Pico GPIO 6	Left motor direction A
IN2	Pico GPIO 7	Left motor direction B
IN3	Pico GPIO 8	Right motor direction A
IN4	Pico GPIO 9	Right motor direction B
ENA	Pico GPIO 10 (PWM)	Enable left motor
ENB	Pico GPIO 11 (PWM)	Enable right motor
12V	Battery positive (e.g., 7.4–12V)	Motor power
GND	Battery GND + Pico GND	Common ground
5V OUT	Optional (to power Pico if jumper is on)	Only if using L298N’s regulator (not recommended for high current)

# Encoder Connections
Encoder	     Pico GPIO	 Description
Left A	     GPIO 2	    Left Encoder Channel A
Left B	     GPIO 3	    Left Encoder Channel B
Right A	     GPIO 4	    Right Encoder Channel A
Right B	     GPIO 5	    Right Encoder Channel B

# Power Connections

    L298N 12V: 12V Battery Positive

    L298N GND: Battery Negative & Pico GND

    Pico VSYS: 5V from L298N or separate 5V supply

#  Software Installation
1. Arduino IDE Setup
   Install Pico Support

    Open Arduino IDE

    Go to File > Preferences

    Add to Additional Boards Manager URLs:
    https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json

        Go to Tools > Board > Boards Manager

    Search for "Raspberry Pi Pico" and install

Install Required Libraries

No external libraries required! All code is self-contained.

LED Behavior Summary
LED	Purpose	Behavior
Built-in LED (GPIO 25)	Power / Heartbeat	Blinks every 0.5s
LED on GPIO 16	ROS connection	ON when ROS is sending commands (active serial)
LED on GPIO 17	Mode indicator	ON = Teleop (manual)
OFF = Autonomous


# Clone my packge and upload the code to pico and make run it .

