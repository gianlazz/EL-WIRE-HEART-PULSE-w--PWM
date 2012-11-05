


volatile int rate[10];                    // used to get running average of HRV values
volatile unsigned long lastBeatTime = 0;  // used to find the time between beats
volatile int sampleCounter;               // used to determine pulse timing
volatile int runningTotal;                // used to keep track of pulses
volatile boolean firstBeat = true;        // used to seed rate array so we startup with reasonable BPM
volatile boolean secondBeat = true;       // used to seed rate array so we startup with reasonable BPM


void interruptSetup(){

  // Initializes Timer1 to throw an interrupt every 1mS.
  TCCR1A = 0x00; // DISABLE OUTPUTS AND PWM ON DIGITAL PINS 9 & 10
  TCCR1B = 0x11; // GO INTO 'PHASE AND FREQUENCY CORRECT' MODE, NO PRESCALER
  TCCR1C = 0x00; // DON'T FORCE COMPARE
  TIMSK1 = 0x01; // ENABLE OVERFLOW INTERRUPT (TOIE1)
  ICR1 = 8000;   // TRIGGER TIMER INTERRUPT EVERY 1mS  
  sei();         // MAKE SURE GLOBAL INTERRUPTS ARE ENABLED
 
}


// THIS IS THE TIMER 1 INTERRUPT SERVICE ROUTINE. 
ISR(TIMER1_OVF_vect){ // triggered every time Timer 1 overflows
  // Timer 1 makes sure that we take a reading every milisecond
  Signal = analogRead(pulsePin);  // read the Pulse Sensor 
  sampleCounter++;                // keep track of the time with this variable (ISR triggered every 1mS

  //  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
  int H = sampleCounter-lastBeatTime;      // monitor the time since the last beat to avoid noise
  if ( (Signal > 520) && (Pulse == false) && (H > 500) ){  
    // signal surges up in value every time there is a pulse    
    Pulse = true;                       // set the Pulse flag when we think there is a pulse
    digitalWrite(13,HIGH);              // turn on pin 13 LED
    HRV = sampleCounter - lastBeatTime; // measure time between beats in mS
    lastBeatTime = sampleCounter;       // keep track of time for next pulse
    if(firstBeat){                      // if it's the first time we found a beat
      firstBeat = false;                // clear firstBeat flag
      return;                           // HRV value is unreliable so discard it
    }   
    if(secondBeat){                     // if this is the second beat
      secondBeat = false;               // clear secondBeat flag
      for(int i=0; i<=9; i++){          // seed the running total to get a realisitic BPM at startup
        rate[i] = HRV;                      
      }
    }                          
    // keep a running total of the last 10 HRV values
    for(int i=0; i<=8; i++){
      rate[i] = rate[i+1];          // shift data in the rate array and drop the oldest HRV value 
    }
  rate[9] = HRV;                    // add the latest HRV to the rate array
  runningTotal = 0;                 // clear the runningTotal variable
  for(int i=0; i<=9; i++){
    runningTotal += rate[i];        // add up the last 10 HRV values
  }  
  runningTotal /= 10;               // average the last 10 HRV values 
  BPM = 60000/runningTotal;         // how many beats can fit into a minute? that's BPM!
  QS = true;                        // set Quantified Self flag when beat is found and BPM gets updated. 
  // QS FLAG IS NOT CLEARED INSIDE THIS ISR
  }                       
 

  if (Signal < 500 && Pulse == true){   // when the values are going down, it's the time between beats
    digitalWrite(13,LOW);               // turn off pin 13 LED
    Pulse = false;                      // reset the Pulse flag so we can do it again!
  }
  
}// end isr




