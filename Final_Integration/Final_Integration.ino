// Integration Watering Arduino Code

#include <Servo.h> // Servo Lib
#include <RF24.h>
#include <SPI.h> // SPI Lib
#include <Wire.h> ;

// Water Sensing and Refilling 
const int levelPin = A0;
const int powerPin = A1; 
const int pumpPin = 5;
const int level;


// IR LOCATOR
int SensorRegister = 0x0E;// 
int HeadingRegister = 0x04; // 
int StrengthRegister = 0x05; //
int heading,strength;

//Servos
Servo P_Servo; // Base Servo 1
Servo M_Servo; // Base Servo 2
Servo E_Servo; // Elbow Servo 

// Servo Positions
int pos = 0;
int Pangle0 = 0;
int Mangle0 = 173; 

//Plant Bytes

byte Check = 1; // sends plant command to check soil moisture  
//byte IRON = 2; // Sends plant command to turn on IR 
RF24 radio(9,10);//originally 7,8 CE, CSN 

//Read and Write Channels for Plant 1
const byte RxPlant1[6] = {"ABCDE"}; // Channel to send to Plant 1 
const byte TxPlant1 [6] = {"00001"};
int text; // communication from Moteino 

// Pump Control [pins 2-4]
// pin numbers
const int stepPin = 3; // Pin 3 -> Stepping Pin for Pump
const int dirPin = 4; // Pin 4 -> Controls Direction of Pump
const int stopPin = 2; // Pin 2 -> High stops power to pump
//const int powerPin = 5; // Power 
// Watering math
float retract = 18.0/1.66; //amount of water that gets evacuated from tube after watering (mL/1.66)
int period = 900; // period of pump (us)
int stepSize = 2;



// Pi Command variable 
int DO; // Command from Pi 











//  ELBOW OBJECT 
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


// ARM 



// SHOULDER OBJECT
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

// DISPENSE WATER 
void rotation(int water = 50) {
  digitalWrite(dirPin,LOW); // Low dirPin is outputting water
  // 200 pulses in one revolution
  float rev = (water + 18.0)/1.66 ; 
  for(int x = 0; x < (200*rev); x++) { 
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(period); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(period); 
  }

   digitalWrite(dirPin,HIGH); // High dirPin is retracting water 

   for(int x = 0; x < (200*retract); x++) { 
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(period); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(period); 
  }
  
}

// Watering Module 
void Watering(int water) {
    
    int angles[2] = {95, 150};
    
    shoulderAngle(P_Servo, M_Servo, 2, angles[0], stepSize);
  
    elbowAngle(E_Servo, 17, angles[1], stepSize);
  
    delay(2000);

    // WATER DISPENSING
    
    digitalWrite(stopPin,LOW); // enable stepper
    rotation(water); // call watering  
    digitalWrite(stopPin,HIGH); // disable stepper 
    
    elbowAngle(E_Servo, angles[1], 17, stepSize); // retract Elbow 
    
    //P is at 175, M is at 0
    shoulderAngle(P_Servo, M_Servo, angles[0], 2, stepSize); // retract Shoulder 
  }




// IR LOCATOR

  void IR_LOCATOR() {
Wire.beginTransmission(SensorRegister);
Wire.write(HeadingRegister); 
Wire.endTransmission();

Wire.requestFrom(SensorRegister,2);


if(Wire.available()<=2){
heading = Wire.read();
strength = Wire.read();
}
}

void setup() {
  // Water Sensing and Refilling 
  pinMode(pumpPin,OUTPUT); // pump signal pin
  pinMode(powerPin,OUTPUT); // power pin for the water level sensor 
  pinMode(levelPin,INPUT); // Analog water level read  
  
  //servo setup
  P_Servo.attach(8,600,2500);  // (pin, min, max)(with P and scratch)
  M_Servo.attach(6,600,2500);
  E_Servo.attach(7,600,2500); //20 is flat, 180 is extended

  // start arm flat
  shoulderAngle(P_Servo, M_Servo, 0, 2, stepSize);
  elbowAngle(E_Servo, 15, 17, stepSize);
  
//Watering Setup
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
  pinMode(stopPin,OUTPUT);
  digitalWrite(stopPin,HIGH);
 // digitalWrite(powerPin,HIGH);

  //RF Setup
  //while (!Serial);
  Serial.begin(9600);
  radio.begin(); 
  radio.openWritingPipe(TxPlant1);
  radio.openReadingPipe(0, RxPlant1);
  radio.stopListening();
  
  // IR Locator Setup 
  Wire.begin();
}


// MAIN LOOP

void loop(){
if (Serial.available() > 0) { 
    DO = Serial.read(); // Command from Pi 

    if (DO == 49){ // RF comm to turn on IR on plant 1 and check for watering 
      
      radio.openWritingPipe(TxPlant1);
      radio.openReadingPipe(0, RxPlant1); 
      for (int i = 0; i<= 10; i++){ // sends to Moteino and checks for a return on each iteration; does it 10 times
        radio.stopListening();
        radio.write(&Check, sizeof(Check));  // writes to Moteino
        delay(1000); // pause so it doesn't listen to itself by accident?
        radio.startListening(); // begins to listen to moteino
        delay(1000); //  // pause so signal can become available
         if (radio.available()){
          break; 
         }
      }
       
      if(radio.available())  { 
        int text = {0}; 
        radio.read(&text, sizeof(text)); // 
          if (text < 0) {
            Serial.println(0); // Write negative value to Pi to skip this plant and move on  
          }
          if (text > 0){
            Serial.println(49); // Write watering amount to Pi to find plant
          }
    
       }
       else{
        Serial.println(50); // Did not turn on RF
       }
    }

     if (DO == 52){
      IR_LOCATOR();
      Serial.println(heading);
     }
     
     if (DO == 53){ // Enter Watering Protocol 
     Watering(text); // pass the watering amount to watering module 
     Serial.println(53); // Let the Pi know that the plant has been watered
    }

    if (DO == 54){ // Enter Docking Protocol 
      digitalWrite(powerPin,HIGH); // powers water level sensor
      digitalWrite(pumpPin,LOW); // Dont water until needed
      delay(100);
      int level = analogRead(levelPin);
      if (level < 600) {
        digitalWrite(pumpPin,HIGH); // Pumping when water below 600
        while (level < 600){
          level = analogRead(levelPin);
        }
        digitalWrite(pumpPin,LOW); // stops when past 600
      }
      }
      
    }
}
