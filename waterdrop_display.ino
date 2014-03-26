// POTENTIOMETER
int potPin = 0;

//Shift register Pin Definitions
int data = 2; 
int latch = 4;
int clock = 6;

//Byte to fill in each shift register
int byte1 = 0;
int byte2 = 0;
int byte3 = 0;
int byte4 = 0;

// LED variables
int timeOn = 2;
long ledMillis = 0;
int LEDstatus[28];
int timeOFF[28];
int timeOff = 50;
long LEDmillis[28];

// WATER PUMP
int pumpPin = 11;
int pumpPin2 = 12;
int pumpState = LOW;
long pumpMillis = 0;
long pumpIntervalOn = 70;
long pumpIntervalOff = 95;

boolean letter = true;

/*
 * When Arduino is on, we set up each pin mode
 * and let the water pump douche
 */
void setup()
{

  pinMode(data, OUTPUT);
  pinMode(clock, OUTPUT);  
  pinMode(latch, OUTPUT);

  pinMode(pumpPin, OUTPUT);
  pinMode(pumpPin2, OUTPUT);

  // init LED status to LOW
  for(int i = 0; i < 28; i++) {
    LEDstatus[i] = LOW;
    timeOFF[i] = 50;
    LEDmillis[i] = 0;
  }

  // init a timeOFF for each LED

  // init water pump
  digitalWrite(pumpPin, HIGH);
  digitalWrite(pumpPin2, HIGH);
  delay(3000);
  digitalWrite(pumpPin, LOW);
  digitalWrite(pumpPin2, LOW);

  Serial.begin(9600);
}

/*
 *
 */
void loop()
{

  unsigned long currentMillis = millis();

  timeOff = analogRead(potPin);
  timeOff = map(timeOff, 0, 1023, 0, 300);
  Serial.println(timeOff);

/*  if (letter) {
    //letterE();
    letter = false;
  }
*/
  controlLEDs(currentMillis);

  // Control pump
  if ((currentMillis - pumpMillis > pumpIntervalOff) && (pumpState == LOW)) {
    pumpMillis = currentMillis;
    pumpState = HIGH;
  }
  else if ((currentMillis - pumpMillis > pumpIntervalOn) && (pumpState == HIGH)) {
    pumpMillis = currentMillis;
    pumpState = LOW;
  }


  // Water pump state
  digitalWrite(pumpPin, pumpState);
  digitalWrite(pumpPin2, pumpState);
}

void controlLEDs(unsigned long currentMillis) {

  int phase = 0;

  for(int i = 0; i < 28; i++) {
    //phase = 1*(i/4);
    //if((i / 4 == 0) || (i/4 ==2 ) || (i/4 == 4)) {
    if (((currentMillis - LEDmillis[i]) + phase > timeOff) && (LEDstatus[i] == LOW)) {
    //if ((currentMillis - LEDmillis[i] > timeOFF[i]) && (LEDstatus[i] == LOW)) {
      LEDstatus[i] = HIGH;
      LEDmillis[i] = currentMillis;
    }
    else if (((currentMillis - LEDmillis[i]) > timeOn) && (LEDstatus[i] == HIGH)) {
      LEDstatus[i] = LOW;
      LEDmillis[i] = currentMillis;
    }

    changeLED(i, LEDstatus[i]);
    //}
  }
}

/*  Bitwise math to change individual LEDs
 *  Notice that the first bite is being ignore, once we're not using it
 *  at the shift registers
 *  So, each shift register has 7 states
 */
int bits[] = {
  B00000010, B00000100, B00001000, B00010000, B00100000, B01000000, B10000000};
int masks[] = {
  B11111101, B11111011, B11110111, B11101111, B11011111, B10111111, B01111111};

/*
 * changeLED(int led, int state) - changes an individual LED 
 */
void changeLED(int led, int state){
  if(led / 7 == 0) {
    byte1 = byte1 & masks[led];  //clears ledState of the bit we are addressing
    if(state == HIGH){
      byte1 = byte1 | bits[led];
    } //if the bit is on we will add it to ledState
  }
  else if(led / 7 == 1) {  //clears ledState of the bit we are addressing
    byte2 = byte2 & masks[led%7];
    if(state == HIGH){
      byte2 = byte2 | bits[led%7];
    } //if the bit is on we will add it to ledState
  }
  else if(led / 7 == 2) {  //clears ledState of the bit we are addressing
    byte3 = byte3 & masks[led%7];
    if(state == HIGH){
      byte3 = byte3 | bits[led%7];
    } //if the bit is on we will add it to ledState
  }
  else if(led / 7 == 3) {  //clears ledState of the bit we are addressing
    byte4 = byte4 & masks[led%7];
    if(state == HIGH){
      byte4 = byte4 | bits[led%7];
    } //if the bit is on we will add it to ledState
  }

  digitalWrite(latch, LOW);     //Pulls the chips latch low
  shiftOut(data, clock, MSBFIRST, byte4); // Shifts out the 8 bits to the 4th shift register
  shiftOut(data, clock, MSBFIRST, byte3); // Shifts out the 8 bits to the 3th shift register
  shiftOut(data, clock, MSBFIRST, byte2); // Shifts out the 8 bits to the 2nd shift register
  shiftOut(data, clock, MSBFIRST, byte1); // Shifts out the 8 bits to the 1st shift register
  digitalWrite(latch, HIGH);   //Pulls the latch high displaying the data
}

void letterE() {

  int shape[28] = {
    1, 1, 1, 1,
    1, 0, 0, 0,
    1, 0, 0, 0,
    1, 1, 1, 1,
    1, 0, 0, 0,
    1, 0, 0, 0,
    1, 1, 1, 1
  };

  for(int i = 0; i < 28; i++) {
    if(shape[i] == 1)
      timeOFF[i] = 0;
    else
      timeOFF[i] = 28;
  }
}

