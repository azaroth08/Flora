#include<pt.h>

const int stepPin1 = 2;   //Motor control pins
const int dirPin1 = 6;
const int stepPin2 = 4;
const int dirPin2 = 5;
char receive[31];         //serial receive variable
int motor1_dir;           //variable to store the motor direction data
int motor2_dir;
const int en = 7;         //motor enable pin : high turns the motors off
const int trigPin = 10;   //Ultrasonic control pins
const int echoPin = 11;
const int trigPin2 = 12;
const int echoPin2 = 13;
int distThresh = 30;      // distance threshold to stop Flora
long duration;            //duration of US pulse, and distance variables
long duration2;
int distance1;
int distance2;
int comm[3];              //variable to hold the parsed serial command
int num_steps;            //number of steps to take
int steps_left;           //steps remaning to take
int steps_start;          //number of steps to begin with
int ramp_steps;           //number of steps to ramp the speed up and down with
unsigned int delayTime;   //the length of pause between each step
int Flag = 0;                 // Flag for US interruption

static struct pt pt1, pt2, pt3;   //protothread structures

static int protoUS1(struct pt *pt) {
  static unsigned long timeStamp = 0;
  PT_BEGIN(pt);         //the first ultrasonic code
  while (1) {
    timeStamp = millis();
    PT_WAIT_UNTIL(pt, millis() - timeStamp > 415);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH, 3000);
    distance1 = duration / 2 * 0.034;
  }
  PT_END(pt);
}

static int protoUS2(struct pt *pt) {
  static unsigned long timeStamp = 0;
  PT_BEGIN(pt);         //the seconds ultrasonic code
  while (1) {
    timeStamp = millis();
    PT_WAIT_UNTIL(pt, millis() - timeStamp > 400);
    digitalWrite(trigPin2, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin2, LOW);
    duration2 = pulseIn(echoPin2, HIGH, 30000);
    distance2 = duration2 / 2 * 0.034;
  }
  PT_END(pt);
}

static int protoMain(struct pt *pt) {
  static unsigned long timeStamp = 0;
  PT_BEGIN(pt);
  while (1) {
    if (Serial.available()) {
      byte sizeR = Serial.readBytes(receive, 31);
      receive[sizeR] = 0;
      char* command = strtok(receive, ",");
      int i = 0;
      while (command != 0)
      { //parses the received serial communication
        //into three separate variables, separated by ","
        comm[i] = atoi(command);
        i += 1;
        command = strtok(0, ",");
      }
    }

    if (comm[0] == 0) {  //sets the motor directions dependent on the serial input
      if (comm[1] == 1) {
        motor1_dir = LOW;
        motor2_dir = HIGH;
      }
      else if (comm[1] == 0) {
        motor1_dir = HIGH;
        motor2_dir = LOW;
      }
    }
    else if (comm[0] == 1) {
      if (comm[1] == 1) {
        motor1_dir = HIGH;
        motor2_dir = HIGH;
      }
      else if (comm[1] == 0) {
        motor1_dir = LOW;
        motor2_dir = LOW;
      }
    }

    num_steps = comm[2];
    comm[2] = 0;
    steps_start = num_steps;
    delayTime = 15000;
    if (num_steps < 200) {  //used when the number of steps is less than the default
      ramp_steps = num_steps / 2; // number of steps in the ramp functions
    }
    else {
      ramp_steps = 100;
    }
    Flag = 0;
    while (num_steps > 0) {
      if (comm[0] == 1) {
        delayTime = 30000;
      }
      else if (comm[0] == 0) {  //ramps up and down or sets the constant delay
        if (num_steps + ramp_steps > steps_start) {
          delayTime = delayTime - 100;
        }
        else if (num_steps < ramp_steps) {
          delayTime = delayTime + 100;
        }
        else {
          delayTime = 5000;
        }
      }
      digitalWrite(en, LOW);                //Takes one step for each motor, with a delay time as defined earlier
      if (num_steps == steps_start) {
        timeStamp = millis();
        PT_WAIT_UNTIL(pt, millis() - timeStamp > 250);
      }

      digitalWrite(dirPin1, motor1_dir);
      digitalWrite(dirPin2, motor2_dir);
      digitalWrite(stepPin1, HIGH);
      digitalWrite(stepPin2, HIGH);
      timeStamp = micros();
      PT_WAIT_UNTIL(pt, micros() - timeStamp > delayTime);
      digitalWrite(stepPin1, LOW);
      digitalWrite(stepPin2, LOW);
      timeStamp = micros();
      PT_WAIT_UNTIL(pt, micros() - timeStamp > delayTime);
      num_steps--;                //increments the number of steps left to take
      if (num_steps == 0) {
        timeStamp = millis();  //adds a delay to the motor shut off.
        PT_WAIT_UNTIL(pt, millis() - timeStamp > 500); // helps reduce drift when stopping
        if (Flag == 1) {
          Serial.println(steps_left);
        }
        else if (Flag == 0){
          Serial.println(0);
        }
      }
      if ( comm[0] != 1 && ((distance1 < distThresh && distance1 != 0) || (distance2 <distThresh && distance2 != 0)) && num_steps > ramp_steps ) {
        steps_left = num_steps - ramp_steps;
        num_steps = ramp_steps;        
        Flag = 1;;
      }
    }
    //shuts off the motor drives when Flora isnt moving

    digitalWrite(en, HIGH);    //saves power and wont melt the drives or wires

  }
  PT_END(pt)

}



void setup() {
  // initialize the output and input pins, serial, and pt threads
  pinMode(stepPin1, OUTPUT);
  pinMode(dirPin1, OUTPUT);
  pinMode(stepPin2, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  Serial.begin(9600);
  digitalWrite(trigPin, LOW);
  digitalWrite(trigPin2, LOW);
  PT_INIT(&pt1);
  PT_INIT(&pt2);
  PT_INIT(&pt3);

}
void loop() {  //continuously loop the pt functions.  See full
  protoMain(&pt1);  //documentation to try and understand because
  //protoUS1(&pt2);   // the concept makes absolutely no sense to me
  //protoUS2(&pt3);
}
