
/*
>> Pulse Sensor Amped <<
This code is for Pulse Sensor Amped by Joel Murphy and Yury Gitman
    www.pulsesensor.com 
    >>> Pulse Sensor purple wire goes to Analog Pin 0 <<<
Pulse Sensor sample aquisition and processing happens in the background via Timer 1 interrupt. 1mS sample rate.
PWM on pins 9 and 10 will not work when using this code!
The following variables are automatically updated:
Pulse :     boolean that is true when a heartbeat is sensed then false in time with pin13 LED going out.
Signal :    int that holds the analog signal data straight from the sensor. updated every 1mS.
HRV  :      int that holds the time between the last two beats. 1mS resolution.
BPM  :      int that holds the heart rate value. derived every pulse from averaging previous 10 HRV values.
QS  :       boolean that is made true whenever Pulse is found and BPM is updated. User must reset.

This code is designed with output serial data to Processing sketch "PulseSensorAmped_Processing-xx"
The Processing sketch is a simple data visualizer. 
All the work to find the heartbeat and determine the heartrate happens in the code below.
Pin 13 LED will blink with heartbeat.
It will also fade an LED on pin 11 with every beat. Put an LED and series resistor from pin 11 to GND

See the README for more information and known issues.
Code Version 0.1 by Joel Murphy & Yury Gitman  Summer 2012     
*/


//  VARIABLES
int pulsePin = 0;          // pulse sensor purple wire connected to analog pin 0
int fadeRate = 0;          // used to fade LED on PWM pin 11

                                    // these are volatile because they are used during the interrupt!
volatile int BPM;                   // used to hold the pulse rate
volatile int Signal;                // holds the incoming raw data
volatile int HRV;                   // holds the time between beats
volatile boolean Pulse = false;     // true when pulse wave is high, false when it's low
volatile boolean QS = false;        // becomes true when pulse rate is determined. every 20 pulses


void setup(){
  pinMode(2, OUTPUT);  // channel A 
  pinMode(3, OUTPUT);  // channel B   
  pinMode(4, OUTPUT);  // channel C
  pinMode(5, OUTPUT);  // channel D    
  pinMode(6, OUTPUT);  // channel E
  pinMode(7, OUTPUT);  // channel F
  pinMode(8, OUTPUT);  // channel G
  pinMode(13,OUTPUT);    // pin 13 will blink to your heartbeat!
  pinMode(11,OUTPUT);    // pin 11 will fade to your heartbeat!
  Serial.begin(115200);  // we agree to talk fast!
  interruptSetup();      // sets up to read Pulse Sensor signal every 1mS 
   // UN-COMMENT THE NEXT LINE IF YOU ARE POWERING THE PulseSensor AT LOW VOLTAGE, 
   // AND APPLY THAT VOLTAGE TO THE A-REF PIN
   //analogReference(EXTERNAL);   
}



void loop(){

  sendDataToProcessing('S', Signal);   // send Processing the raw Pulse Sensor data
  
  if (QS == true){                     // Quantified Self flag is true when arduino finds a heartbeat
    fadeRate = 255;                    // Set 'fadeRate' Variable to 255 to fade LED with pulse
    sendDataToProcessing('B',BPM);     // send the time between beats with a 'B' prefix
    sendDataToProcessing('Q',HRV);     // send heart rate with a 'Q' prefix
    QS = false;                        // reset the Quantified Self flag for next time    
   }

   ledFadeToBeat();  
   
   delay(20);                          //  take a break

}


void ledFadeToBeat(){
  fadeRate -= 15;                         //  set LED fade value
  fadeRate = constrain(fadeRate,0,255);   //  keep LED fade value from going into negative numbers!
  analogWrite(2,fadeRate);               //  fade EL Wire
  analogWrite(11,fadeRate);               //  fade LED
}


void sendDataToProcessing(char symbol, int data ){
  Serial.print(symbol);      // symbol prefix tells Processing what type of data is coming
  Serial.println(data);      // the data to send
}







