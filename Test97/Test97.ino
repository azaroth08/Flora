/*
     Servo Motor Control using the Arduino Servo Library
           by Dejan, https://howtomechatronics.com
*/

#include <Servo.h>

Servo P_Servo;  // create servo object to control a servo
Servo M_Servo;
Servo E_Servo;

int pos = 0;
int Pangle0 = 0;
int Mangle0 = 175;

void setup() {
  P_Servo.attach(9,600,2500);  // (pin, min, max)(with P and scratch)
  M_Servo.attach(10,600,2500);
  E_Servo.attach(11,600,2500); //20 is flat, 180 is extended
}

void loop() {

  P_Servo.write(Pangle0);  // tell servo to go to a particular angle
  M_Servo.write(Mangle0); 
  delay(2000);

  for(pos = 0; pos<=90; pos+=2)
  {
    P_Servo.write(pos);
    M_Servo.write(Mangle0-pos);
    delay(200);
  }
  //P is at 175, M is at 0
  for(pos=175; pos>=0; pos-=2)
  {
    P_Servo.write(pos);
    M_Servo.write(175-pos);
    delay(200);
  }

  //P_Servo.write(0);  // tell servo to go to a particular angle
  //M_Servo.write(180); 
  //delay(2000);
  /*
  E_Servo.write(20);
  delay(4000);
  E_Servo.write(180);
  delay(4000);

  P_Servo.write(90);  // tell servo to go to a particular angle
  M_Servo.write(90); 
  delay(1000);
  
  for(pos = 15; pos<=180; pos+=5)
  {
    E_Servo.write(pos);
    delay(500);
  }
  for(pos=180; pos>=15; pos-=5){
    E_Servo.write(pos);
    delay(500);
  }
  */
}
