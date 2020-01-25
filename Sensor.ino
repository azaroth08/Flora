Code for Flora (Receiver): 
//Receiver Code - Arduino Yun

//Need these libraries
#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>
 
RF24 radio(8,10);
 
const byte rxAddr[6] = "ABCDE";//Used this channel to avoid interference.  
 
void setup()
{ 
  while (!Serial);
  Serial.begin(9600);
  Serial.println("Begin Testing"); 
  radio.begin();
  radio.openReadingPipe(0, rxAddr);
  radio.startListening(); 
}
 
void loop()
{
  if (radio.available())
  {  
    int text = {0};//This is the variable we will be expecting from the transmitter
    radio.read(&text, sizeof(text));
    int moistureAverage = text; //Since "text" is really moistureAverage, we will assign the value to a new variable.
    
    if (moistureAverage == 0) //The soil moisture sensor is not in properly, thus there is no connection.
    { 
      Serial.println("Please ensure that the sensor is placed in the soil correctly, as it is reading a value of 0."); 
    } 
    else 
    {  
    Serial.println("The soil moisture is:");
    Serial.println(moistureAverage); 
    } 
  }  
}

Code for Plant Sensor (Transmitter):
//Transmitter Code - Uno
#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h> 
 
#define sensorPower 2 //The soil sensor will be tested using digital pin 2.
#define moistureReader A0  //Analog pin 0 will will receive the signal from the soil sensor.
int moisture = 0; //Will store the immediate moisture.
int totalMoisture = 0; //Will store the total moisture of N iterations.
int moistureAverage = 0; //Will store the average moisture of N iterations.
byte demoIterations = 5; //The number of measurements to be taken (referred to as 'N' in comments above.
 
RF24 radio(8,10);
 
const byte rxAddr[6] = "ABCDE";
 
void setup()
{ 
  //pinMode(10,OUTPUT);
  while (!Serial);
  Serial.begin(9600);
 
  pinMode(sensorPower,OUTPUT);  
  pinMode(moistureReader, INPUT); 
 
  radio.begin();
  radio.setRetries(15, 15);
  radio.openWritingPipe(rxAddr);
  radio.stopListening(); 
  
}
 
void loop()
{ 
  
  //const char text[] = "They work!";  
  //const int text[] = "They work!";  
 
  for (int a = 0; a <5; a++)//Take five measurements so the average can be calculated before 
{
  digitalWrite(sensorPower,HIGH); 
  delay(100);  
  moisture = analogRead(moistureReader); 
  delay(100);
  digitalWrite(sensorPower,LOW); 
  totalMoisture= totalMoisture+moisture; 
}   
  moistureAverage = totalMoisture/5; //Calculating the average 
  const int text = moistureAverage; //Will be sending the moistureAverage to the Receiver
 
  for (int b = 0; b<demoIterations; b++) //This is done for the sake of the milestone demo, where the goal was to transmit 
  //data 4 times and ensure that the receiver was able to obtain the data. 
  
  {   
  radio.write(&text, sizeof(text)); 
  delay(500);  
  } 
  
  //Resettng all values
  moistureAverage = 0; 
  totalMoisture = 0; 
  moisture = 0;
}
