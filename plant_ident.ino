//This code is used to demonstrate a simple, bare-bones GUI for two plant types. 
//This will suffice for module demo, but will later be upgraded for future demos. 

#define LED 2 

void setup(){ 
Serial.begin(9600);   
Serial.println("Please specify the plant. Type 'a' for cactus or 'b' for succulent.");  
//The user will input an a or a b for the type of plant they will be pairing with the sensor. 
}  

 

void loop() {  

  if(Serial.available() > 0) //If the serial monitor is available 
  {  
   if(Serial.peek() == 'a') {  
    Serial.read();  
    digitalWrite(LED,HIGH);   
    Serial.println("This sensor is for a cactus!"); 
   }    

   else if(Serial.peek() == 'b') {  
    Serial.read();  
    digitalWrite(LED,LOW);   
    Serial.println("This sensor is for a succulent"); 
   } 
  }  

} 
