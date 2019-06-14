/*
Joanna and Andy (and Dimitri for the last 2 days)
Due June 14, 2019

A program to control the Arduino robot car.
*/


#include <NewPing.h> // library to operate ultrosonic sensor

// pin constants
// MOTOR_PINS[m][w] denotes the motor m:
// m == 1: left motor; m == 0: right motor
// w == 0: digital wire; w == 1: analog wire
#define LEFT 1
#define RIGHT 0
const int MOTOR_PINS[2][2] = {{2, 3}, {4, 5}};

// pin for line detection: LOW is black, HIGH is other
#define SENSOR_PIN 8;

// constants for linear and rotational motion
/*
Note: the right motor tends to spin a little bit faster than
the left, so these constants are to offset this hardware
imperfection.
*/
const double FORWARD_IMBALANCE = 1.04;
const double BACKWARD_IMBALANCE = 1.14;
const double ROTATION_TIME = 7.2;

// delay time between actions
const int DELAY_TIME = 1000;

// analog values for full and half speed
const int FULL_SPEED = 150;
const int HALF_SPEED = 100;

// how often we look for a line in the follow_line_test() function
const int INCREMENT = 80;

// setup for ultrasonic sensor
// TODO: ensure pin values are correct
#define TRIGGER_PIN A1
#define ECHO_PIN A2
NewPing sonar(TRIGGER_PIN, ECHO_PIN);

// for wall detection: centimetres before switching direction
const int THRESHOLD = 12;


// --------------------UTILITY FUNCTIONS--------------------

void spin_motor(int m, int s) {
  if (m == LEFT) {
    // the left motor is slower, so scale it depending
    // on if the car is to move forward or backward
    s *= (s >= 0 ? FORWARD_IMBALANCE : BACKWARD_IMBALANCE);
  }
  // use math to move forward and backward
  if (s >= 0) {
    digitalWrite(MOTOR_PINS[m][0], LOW);
    analogWrite(MOTOR_PINS[m][1], s);
  } else {
    digitalWrite(MOTOR_PINS[m][0], HIGH);
    analogWrite(MOTOR_PINS[m][1], 255 + s);
  }
}

void spin_motors(int left_speed, int right_speed) {
  spin_motor(LEFT, left_speed);
  spin_motor(RIGHT, right_speed);
}

void spin_motors_timed(int left_speed, int right_speed, int milliseconds) {
  int start_time = millis();
  int end_time = start_time + milliseconds;
  while (millis() < end_time) { // loop while end time is not reached
    spin_motors(left_speed, right_speed);
  }
}

void spin_motors_timed(int s, int milliseconds) {
  spin_motors_timed(s, s, milliseconds);
}

void turn_motors_off(int milliseconds) {
  spin_motors_timed(0, milliseconds);
}

void rotate_car(int d) {
  if (d == 0) {
    return;
  }
  int duration = abs(ROTATION_TIME * d);
  if (d > 0) {
    spin_motors_timed(-100, 100, duration);
  } else {
    spin_motors_timed(100, -100, duration);
  }
}

void go_full_speed(int milliseconds) {
  spin_motors_timed(FULL_SPEED, milliseconds);
}

void go_half_speed(int milliseconds) {
  spin_motors_timed(HALF_SPEED, milliseconds);
}


// --------------------SUBMISSION 2 CODE--------------------

void speed_test() {
  // goes full speed for 2 seconds, then half speed for 2 seconds
  go_full_speed(2000);
  turn_motors_off(DELAY_TIME);
  go_half_speed(2000);
}

void straight_line_test() {
  go_full_speed(1500);
}

void rotate_test() {
  // the argument to rotate_car is -90 because
  // clockwise motion corresponds to a negative degree value
  rotate_car(-90);
  turn_motors_off(DELAY_TIME);
  rotate_car(-90);
  turn_motors_off(DELAY_TIME);
  rotate_car(-90);
  turn_motors_off(DELAY_TIME);
  rotate_car(-90);
}


// --------------------SUBMISSION 3 CODE--------------------

void detect_line_test() {
  // does a 360 when a black line is detected
  while (digitalRead(SENSOR_PIN) == HIGH) { // non-black was detected
    spin_motors(HALF_SPEED, HALF_SPEED);
  }
  rotate(360);
}

void follow_line_test(int milliseconds) {
  // line follower code
  int start_time = millis();
  int end_time = start_time + milliseconds;
  while (millis() < end_time) {
    // NOTE: LOW means black was detected
    if (digitalRead(SENSOR_PIN) == LOW) {
      // move right (spin left motor)
      spin_motors_timed(HALF_SPEED, 0, INCREMENT);
    } else {
      spin_motors_timed(0, HALF_SPEED, INCREMENT);
    }
  }
}


// --------------------SUBMISSION 4 CODE--------------------

void obstacle_test() {
  // detection for obstacles; does a 360 once obstacle is detected
  while (sonar.ping_cm() > THRESHOLD) {
    spin_motors(HALF_SPEED, HALF_SPEED);
  }
  rotate_car(360);
}

void ping_pong_test(int milliseconds) {
  int start_time = millis();
  int end_time = start_time + milliseconds;
  while (millis() < end_time) { // loop while end time is not reached
    if (sonar.ping_cm() > THRESHOLD) {
      spin_motors(HALF_SPEED, HALF_SPEED); // not close to a wall
    } else {
      rotate_car(180); // too close to wall; turn around
    }
  }
}

void boundary_test() {
  // ensure lap direction is counterclockwise
  for (int i = 0; i < 4; i++) {
    while (sonar.ping_cm() > THRESHOLD) { // not close to wall
      spin_motors(HALF_SPEED, HALF_SPEED);
    }
    rotate_car(90); // turn left
  }
}

void setup() {
  // set pin modes
  for (int m = 0; m < 2; m++) {
    for (int w = 0; w < 2; w++) {
      pinMode(MOTOR_PINS[m][w], OUTPUT);
    }
  }
  pinMode(SENSOR_PIN, INPUT);
//  speed_test();
//  straight_line_test();
//  rotate_test();
  follow_line_test(30000);
  //obstacle_test();
}

void loop() {
  spin_motors(0, 0); // keep motors off
}
