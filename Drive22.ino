
/*     Simple Stepper Motor Control Exaple Code
 *      
 *  by Dejan Nedelkovski, www.HowToMechatronics.com
 *  
 */
// defines pins numbers
const int stepPin1 = 2; 
const int dirPin1 = 6; 
const int stepPin2 = 4;
const int dirPin2 = 5;
const int sleep = 8;

char receive[31];

int motor1_dir= HIGH;
int motor2_dir= HIGH;
const int en = 7;

const int trigPin = 10;
const int echoPin = 11;
const int trigPin2 = 12;
const int echoPin2 = 13;

int distThresh = 25;

long duration;
long duration2;
int distance1;
int distance2;
int count=0;
int comm[3];

int num_steps;
int steps_left;
unsigned int delayTime;



 
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
  Serial.begin(9600);



  
}
void loop() {
  digitalWrite(trigPin,LOW);
  digitalWrite(trigPin2,LOW);
  
  
  if (Serial.available()){
    byte size = Serial.readBytes(receive,31);
    //receive[size] = 0;
    char* command = strtok(receive, ",");
    int i = 0;
    while (command != 0)
    {

        comm[i] = atoi(command);
        i+=1;
    // Find the next command in input string
        command = strtok(0, ",");
    }
    //Serial.println(comm[0]);
    //Serial.println(comm[1]);
    //sSerial.println(comm[2]);
  }

  if (comm[0] == 0){
    if (comm[1] == 1){
      motor1_dir = LOW;
      motor2_dir = HIGH;
    }
    else if (comm[1] == 0){
      motor1_dir = HIGH;
      motor2_dir = LOW;
    }
  }
  else if (comm[0] == 1) {
    if (comm[1] ==1) {
      motor1_dir = HIGH;
      motor2_dir = HIGH;
    }
    else if (comm[1] == 0){
      motor1_dir = LOW;
      motor2_dir = LOW;
    }
  }

  num_steps = comm[2];
  comm[2] = 0;
  count = 1;
  

  while (num_steps >0){
    digitalWrite(en,LOW);                 //Takes one step for each motor, with a delay time as defined earlier
    digitalWrite(dirPin1,motor1_dir);    
    digitalWrite(dirPin2,motor2_dir);
    digitalWrite(stepPin1,HIGH); 
    digitalWrite(stepPin2,HIGH); 
    delayMicroseconds(delayTime); 
    digitalWrite(stepPin1,LOW); 
    digitalWrite(stepPin2,LOW); 
    delayMicroseconds(delayTime); 
    num_steps--;                        //increments the number of steps left to take 
    Serial.println(num_steps);
    
    if (count ==41){                    //checks for an object in front of flora and its distance on the first 
        count = 0;                      //sensor
        digitalWrite(trigPin,HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin,LOW);

        duration = pulseIn(echoPin,HIGH,15000);

        distance1 = duration/2*0.034;
    }
    else if (count ==21){               //checks hte second ultrasonic sensor and the distance
        delayMicroseconds(20);
        digitalWrite(trigPin2,HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin2,LOW);

        duration2 = pulseIn(echoPin2,HIGH,5000);

        distance2 = duration2/2*0.034;
    }
        
    count++;
    if ((distance1 <= distThresh && distance1!=0) || (distance2 <=distThresh && distance2 != 0)) {
      steps_left = num_steps;
      num_steps = 20;           
    }
    else {
      steps_left = num_steps;
    }
  }
               //shuts off the motor drives when Flora isnt moving
    
  digitalWrite(en,HIGH);              //saves power and wont melt the drives or wires
  Serial.println(steps_left
}
