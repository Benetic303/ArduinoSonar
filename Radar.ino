#include <Servo.h>

Servo myServo; // Create a servo object to control a servo motor

int servoPin = 9; // Define the pin to which the servo is connected
const int echo = 13;
const int trig = 12;

int duration=0;
int distance=0;

void setup() {
  myServo.attach(servoPin); // Attaches the servo on pin 9 to the servo object
  pinMode(trig,OUTPUT);
  pinMode(echo,INPUT); 
  Serial.begin(9600);
}

void loop() {
  // Sweep the servo from 0 to 180 degrees in steps of 1 degree
  for (int angle = 0; angle <= 180; angle++) {
    myServo.write(angle); // Set the servo position
    
    digitalWrite(trig,HIGH);
    delay(10);
    digitalWrite(trig,LOW);

    long duration = pulseIn(echo,HIGH);
    int distance = (duration * 0.034)/2;
    Serial.println("");
    delay(40); // Wait for servo to reach the position
    Serial.print("A: "); //A: for angle
    Serial.print(angle); // Sends the current degree into the Serial Port
    Serial.print(" D: "); //D for distance
    //Serial.print(","); //D for distance
    Serial.print(distance); // Sends the distance value into the Serial Port
  

  }

  
  // Sweep the servo from 180 to 0 degrees in steps of 1 degree
  for (int angle = 180; angle >= 0; angle--) {
    myServo.write(angle); // Set the servo position
    digitalWrite(trig,HIGH);
    delay(10);
    digitalWrite(trig,LOW);

    duration = pulseIn(echo,HIGH);
    distance = (duration/2)/28.5;
    delay(40); // Wait for servo to reach the position
    Serial.println("");
    Serial.print("A: "); //A: for angle
    Serial.print(angle); // Sends the current degree into the Serial Port
    Serial.print(" D: "); //D for distance
    //Serial.print(","); //D for distance
    Serial.print(distance); // Sends the distance value into the Serial Port

  }
}

