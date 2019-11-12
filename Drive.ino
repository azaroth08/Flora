/*
* Drives the base including the ultrasonic sensor integration
* Receives commands via serial using wasd to drive, and e to set the drivers to sleep.
*/

// defines pins numbers
const int stepPin1 = 2; 
const int dirPin1 = 3; 
const int stepPin2 = 4;
const int dirPin2 = 5;
const int sleep = 8;
bool go = false;
char receive = 'w';
char dir = receive;
int motor1_dir= HIGH;
int motor2_dir= HIGH;
const int en = 7;

const int trigPin = 10;
const int echoPin = 11;
const int trigPin2 = 12;
const int echoPin2 = 13;

int distThresh = 10;

long duration;
long duration2;
int distance1;
int distance2;
int count=0;


 
void setup() {
  // Sets the two pins as Outputs
  pinMode(stepPin1,OUTPUT); 
  pinMode(dirPin1,OUTPUT);
  pinMode(stepPin2,OUTPUT); 
  pinMode(dirPin2,OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin,INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2,INPUT);
  Serial.begin(115200);
}
void loop() {
  digitalWrite(trigPin,LOW);
  digitalWrite(trigPin2,LOW);
  if (Serial.available()>0) {
    receive = Serial.read();
  }  re

  if (receive == 'w') {
    digitalWrite(sleep,HIGH);
    digitalWrite(en,LOW);
    motor1_dir = HIGH;
    motor2_dir = LOW;
    go = true;
  }
  else if (receive == 's') {
    digitalWrite(sleep,HIGH);
    digitalWrite(en,LOW);
    motor1_dir = LOW;
    motor2_dir = HIGH;
    go = true;
  }
  else if (receive == 'a') {
    digitalWrite(sleep,HIGH);
    digitalWrite(en,LOW);
    motor1_dir = LOW;
    motor2_dir = LOW;
    go = true;
  }
  else if (receive == 'd') {
    digitalWrite(sleep,HIGH);
    digitalWrite(en,LOW);
    motor1_dir = HIGH;
    motor2_dir = HIGH;
    go = true;
  }
  else if (receive == 'q') {
    digitalWrite(sleep,LOW);
    digitalWrite(en,LOW);
    motor1_dir = HIGH;
    motor1_dir = LOW;
    go = true;
  }
  else if (receive == 'e') {
    digitalWrite(en,HIGH);
    digitalWrite(sleep,LOW);
    go = true;
  }
  if (go) {
    digitalWrite(dirPin1,motor1_dir); 
    digitalWrite(dirPin2,motor2_dir);
    digitalWrite(stepPin1,HIGH); 
    digitalWrite(stepPin2,HIGH); 
    delayMicroseconds(5000); 
    digitalWrite(stepPin1,LOW); 
    digitalWrite(stepPin2,LOW); 
    delayMicroseconds(5000); 
  }
  
  if (count ==50){
    count = 0;
    digitalWrite(trigPin,HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin,LOW);

    duration = pulseIn(echoPin,HIGH,15000);

    distance1 = duration/2*0.034;
    delayMicroseconds(20);
    digitalWrite(trigPin2,HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin2,LOW);

    duration2 = pulseIn(echoPin2,HIGH,3000);

    distance2 = duration2/2*0.034;
    
    Serial.print(distance1);
    Serial.print("\t");
    Serial.print(distance2);
    Serial.print("\t");
    Serial.println(dir);
    
  }
  count++;

  if (distance1 <= distThresh && distance1 > 0 || distance2 >=8) {
    receive = 'e';
  }
  
  
}
