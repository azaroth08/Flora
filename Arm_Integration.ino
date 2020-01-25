#include <Servo.h>

// create servo object to control a servo
Servo R_Servo;  // Right shoulder servo
Servo L_Servo;  // Left shoulder servo
Servo E_Servo;  // Elbow servo

int pos = 0;
int Pangle0 = 0;
int Mangle0 = 173;

// Pump Control
// pin numbers
const int stepPin = 3;  // Pin 3 -> Stepping Pin for Pump
const int dirPin = 4;   // Pin 4 -> Controls Direction of Pump
const int stopPin = 2;  // Pin 2 -> High stops power to pump
const int powerPin = 5; // Power 
unsigned long Time1;
unsigned long Time2;

int y = 0; // stopping criteria for testing
float trueml = 10.0;
float ml = 18.0 + trueml; //mL of water
float rev = ml/1.66; // number of revolutions

float ml2 = 80.0;
float rev2 = ml2/1.66;
int period = 900; // period of pump (us)

void setup()
{
  delay(2000);
  R_Servo.attach(9,600,2500);  // (pin, min, max)
  L_Servo.attach(10,600,2500);
  E_Servo.attach(11,600,2500); //20 is flat, 180 is extended
/*
  R_Servo.write(20);  // tell servo to go to a particular angle
  L_Servo.write(153);
  E_Servo.write(15);
  */
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
  pinMode(stopPin,OUTPUT);
  pinMode(powerPin,OUTPUT);
  digitalWrite(stopPin,HIGH);
  digitalWrite(powerPin,HIGH);
  
  //Serial.begin(9600);
}

//  Functions
void elbowAngle(Servo e, int startAngle, int endAngle, int stepSize)
{
  if(startAngle > endAngle){
    for(pos = startAngle; pos >= endAngle; pos-= stepSize){  //fully lower elbow joint
      e.write(pos);
      delay(20*stepSize);
    }
  }
  else{
    for(pos = startAngle; pos <= endAngle; pos+= stepSize){  //fully lower elbow joint
      e.write(pos);
      delay(20*stepSize);
    }
  }
}

void shoulderAngle(Servo p, Servo m, int startAngle, int endAngle, int stepSize)
{
  if(startAngle > endAngle){  // retracting
    for(pos = startAngle; pos >= endAngle; pos -= stepSize)  
    {
      p.write(pos);
      m.write(Mangle0-pos);
      delay(20*stepSize);
    }
  }
  else{   //  extending
    for(pos = startAngle; pos <= endAngle; pos += stepSize) 
    {
      p.write(pos);
      m.write(Mangle0-pos);
      delay(20*stepSize);
    }
  }
}

void water()
{
  digitalWrite(dirPin,LOW); // High is one direction, Low is other 
  // 200 pulses in one revolution
  //Time1 = millis();
  for(int x = 0; x < (200*rev); x++)
  { 
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(period); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(period); 
  }

   digitalWrite(dirPin,HIGH); // High is one direction, Low is other 

   for(int x = 0; x < (200*rev2); x++)
  { 
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(period); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(period); 
  }
}

//  Main script
int t = 0;

void loop() 
{
  if(t==0){
    int stepSize = 2;
    // matrix from output of MATLAB sim
    int coords[4][4] = {{10, 30, 64, 135}, // (x, y, shoulder, elbow) values for each test scenario
                        {10, 40, 67, 124},
                        {10, 50, 62, 110},
                        {10, 60, 50, 82}};
    
    

    int test = 3;   // EDIT HERE TO SELECT TARGET HEIGHT
                    // Picking which row of coords to use
    
    int angles[3];
    for(int i=0; i<2; i++){
      angles[i] = coords[test][2+i];
    }
    
    //for(int itt = 0; itt<3; itt++){
      shoulderAngle(R_Servo, L_Servo, 0, angles[0], stepSize);
  
      elbowAngle(E_Servo, 15, angles[1], stepSize);
    
      //add watering call here
      delay(2000);
        
      // watering !!!!!!!!!!!!!!!!!!!!!
  
      if (y == 0)
      {
        digitalWrite(stopPin,LOW);
        //water();                  // comment out if testing position
        y++;
        digitalWrite(stopPin,HIGH);
      }
      else {
        }
      
      elbowAngle(E_Servo, angles[1], 15, stepSize);
      
      //P is at 175, M is at 0
      shoulderAngle(R_Servo, L_Servo, angles[0], 0, stepSize);
    //}
    
    t++;
  }
  
  else{
    E_Servo.detach();
    R_Servo.detach();
    L_Servo.detach();
  }
}
