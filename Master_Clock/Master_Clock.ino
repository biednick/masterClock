/************************************************
 * Master Clock Source Code
 * Written by Nick Biederman
 * Released under CC 4.0 Attribution-Share Alike
 * See github.com/biednick/masterClock for documentation and hardware
 * 
 * V.0.0: 8JUNE2018 Bulk of code written; no hardware to test on at this time. 
 ************************************************/
/*******************************************************************************************************************************************************************/
#define DIV1 7       //Set up pins for various output
#define DIV2 8
#define DIV3 9
#define DIV4 10
#define MULT2 A3
#define MULT3 A2
#define MULT4 A1
#define OUT 4

#define POT A0      //Potentiometer pin

#define RED 5       //Pins for RB LED; ised to show offet value and mode
#define BLUE 6

#define INTER 3     //Interupt pin, used for external clock

#define BUTTON 2    //Pin for tap tempo button

#define DEBOUNCE 50 //Delays for debouncing and changing mode
#define MODEDELAY 250

unsigned long int riseTime, fallTime, first, second;                      //Times for various state changes(button press and external clock in)
unsigned long int lastDiv1 = 0, lastDiv2 = 0, lastDiv3 = 0, lastDiv4 = 0, //time of last state change for each divition
                  lastMult1 = 0, lastMult2 = 0, lastMult3 = 0, lastMult4 = 0, lastOut = 0; 
unsigned long int lastDebounce = 0, lastModeChange = 0;                   //Time values used for delays
unsigned long int fellTime;                                               //Time of last button state change from high to low; used to dertermine how long the button has been low

unsigned int clockRaw, clockTime, clockDiv2, clockDiv3, clockDiv4, clockMult2, clockMult3, clockMult4; //Time between clock pulses for each divition

bool firstSecond = true;                                                       //Flag for button presses
bool fellFlag = false, mode = true;                                            //Flag to change between tap tempo and knob/external
bool div1flag = false, div2flag = false, div3flag = false, div4flag = false,   //Flags for clock state of each divition
     mult2flag = false, mult3flag = false, mult4flag = false, outFlag = false;

int offsetRaw, offset;            //Value of offset used for modifying clock time in tap tempo mode
int lastButtonState, buttonState; //Flags used to determine if the button fell
void setup() {
    riseTime = 0;            //Initialize values for timers; Defaults to 1/4 sec pulses
    fallTime = 250;         
    first = 0;
    second = 250;
    
    pinMode(DIV1, OUTPUT);   //Pinmode for each divition
    pinMode(DIV2, OUTPUT);
    pinMode(DIV3, OUTPUT);
    pinMode(DIV4, OUTPUT);
    pinMode(MULT2, OUTPUT);
    pinMode(MULT3, OUTPUT);
    pinMode(MULT4, OUTPUT);
    pinMode(OUT, OUTPUT);    //Output clock pin, used internally.
    
    pinMode(BUTTON, INPUT_PULLUP);  //Button pin, pulled hgh to prevent floating
    pinMode(INTER, INPUT_PULLUP);   //Interrupt pin, pulled hogh to avoid floating
    
    attachInterrupt(digitalPinToInterrupt(INTER), rise, RISING);  //Inerupt pin set up. Two functions are used to determine the differece between a rising and falling edge.
    attachInterrupt(digitalPinToInterrupt(INTER), fall, FALLING);
}
void loop() {
  
  fellFlag = fell(BUTTON);
  
  if(digitalRead(BUTTON) == LOW && lowTime(BUTTON) > 3000 && millis() - lastModeChange > MODEDELAY){ //Change modes
    mode = !mode;                       //if the button is low, has been low for 3 sec, and mode hasn't recently changed
    lastModeChange = millis();          //reset timer
  } 
  
  if(mode){             //Tap tempo mode
    tapTempo();
  }
  else{                 //External or pot clock mode
    internalExternal();
  }
  
  clockMath();          //Calculate the time for each divition  
  clockChange();        //Turn clocks on and off
}
int scale(int in){
  return ((in/256) + .00001);   //If the pot is below half way increase speed, else decrease speed
}
void rise(){
  riseTime = millis();          //Set rise time to time interupt pin rises
}
void fall(){
  fallTime = millis();          //Set fall time to time interrupt pin falls
}
int lowTime(int pin){      
  if (digitalRead(pin) == HIGH){    //If the button is high low time is < 0
    return -1;
  }
  else{         
    return (millis() - fellTime);   //Calculate low time as current time - last falling time.
  }
}
void tapTempoRead(){
  lastDebounce = millis();          //Debounce
  if(firstSecond){                  //Check flag to determine which value to edit
    first = millis();               //Set value to current time
    firstSecond = !firstSecond;     //change flag
  }
  else{
    second = millis();              //Set value to current time
    firstSecond = !firstSecond;     //change flag
  }
}
 void tapTempo(){
    offsetRaw = analogRead(POT) / 2;    //Read the pot for offset, scale by .5
    offset = scale(offsetRaw);          //Scale offset to usable value (.00001-2)
    clockRaw = abs(first - second);     //Calcualter clockTime from button presses
    clockTime = clockRaw * offset;      //Scale by offset
    if (offsetRaw < 512){
      analogWrite(RED, offsetRaw/2);    //Set LED brigtness and color based on offset
      analogWrite(BLUE, 255);
    }
    else if (offsetRaw > 512){
      analogWrite(BLUE, offsetRaw - 512);
      analogWrite(RED, 255);
    }
    else{
      analogWrite(BLUE, 255);
      analogWrite(RED, 255);
    }
    if (fellFlag && millis() - lastDebounce > DEBOUNCE){  //Read new tempo times
      tapTempoRead();
    }
 }
void internalExternal(){
  clockRaw = analogRead(POT) + 1;       //Read time based on pot value+
    if(millis() - lastOut > clockRaw){
      if(outFlag){                      //Write internal clock
        digitalWrite(OUT, LOW);         //This is connected to the switched pin of the clock in jack
        outFlag = ! outFlag;            //Which allows an internal or external clock to be used
        lastOut = millis();             //without any additonal coding to detect if an external clock is connected. 
      }
      else{
        digitalWrite(OUT, HIGH);
        outFlag = !outFlag;
        lastOut = millis();
      }
    }
    clockTime = abs(riseTime - fallTime);
    analogWrite(RED, 0);                //Set the LED to magenta
    analogWrite(BLUE, 0);
}
bool fell(int pin){                //Determine if button went from high to low
  buttonState = digitalRead(pin);  //Current state
  if (buttonState == LOW && lastButtonState == HIGH){  //If button is high i didn't fall. If the last time it was read was low id din't fall.
    fellTime == millis();          //Set fall timer to calculate low time
    lastButtonState = buttonState; //Set last button state for next check
    return true;
  }
  lastButtonState = buttonState;   //The button didn't fall so no timer needs to be set. Set last button state and return false.
  return false;
}
void clockMath(){              //Calculate clock times for each divition.
  clockDiv2 = clockTime * 2;   //This may seem backwards, but it matches the convention used by other clock multipliers and dividers.
  clockDiv3 = clockTime * 3;   //Dividing a clock means increasing the time between state changes
  clockDiv4 = clockTime * 4;
  clockMult2 = clockTime / 2;  //While multiplying the clock means reducing th time betweens state changes. 
  clockMult3 = clockTime / 3;
  clockMult4 = clockTime / 4;
}
void clockChange(){                     //Change the state of each output
  if(millis() - lastDiv1 > clockTime){  //Check and see if the clock has changed state within th divition time
    if(div1flag){                       //If the clock is currently high
      digitalWrite(DIV1, LOW);          //Set it to low
      div1flag = !div1flag;             //Change the flag
      lastDiv1 = millis();              //And update the timer
    }
    else{                               //Otherwise the clock is low
      digitalWrite(DIV1, HIGH);         //Set it to high
       div1flag = !div1flag;            //Change the flag
       lastDiv1 = millis();             //And update the timer
    }
  }
  if(millis() - lastDiv2 > clockDiv2){  //All of these function the same way as the first one
    if(div2flag){
      digitalWrite(DIV2, LOW);
      div2flag = !div2flag;
      lastDiv2 = millis();
    }
    else{
      digitalWrite(DIV2, HIGH);
       div2flag = !div2flag;
       lastDiv2 = millis();
    }
  }
  if(millis() - lastDiv3 > clockDiv3){
    if(div3flag){
      digitalWrite(DIV3, LOW);
      div3flag = !div3flag;
      lastDiv3 = millis();
    }
    else{
      digitalWrite(DIV3, HIGH);
       div3flag = !div3flag;
       lastDiv3 = millis();
    }
  }
  if(millis() - lastDiv4 > clockDiv4){
    if(div4flag){
      digitalWrite(DIV4, LOW);
      div4flag = !div4flag;
      lastDiv4 = millis();
    }
    else{
      digitalWrite(DIV4, HIGH);
       div4flag = !div4flag;
       lastDiv4 = millis();
    }
  }
  if(millis() - lastMult2 > clockMult2){
    if(mult2flag){
      digitalWrite(MULT2, LOW);
      mult2flag = !mult2flag;
      lastMult2 = millis();
    }
    else{
      digitalWrite(MULT2, HIGH);
       mult2flag = !mult2flag;
       lastMult2 = millis();
    }
  }
  if(millis() - lastMult3 > clockMult3){
    if(mult3flag){
      digitalWrite(MULT3, LOW);
      mult3flag = !mult3flag;
      lastMult3 = millis();
    }
    else{
      digitalWrite(MULT3, HIGH);
       mult3flag = !mult3flag;
       lastMult3 = millis();
    }
  }
  if(millis() - lastMult4 > clockMult4){
    if(mult4flag){
      digitalWrite(MULT4, LOW);
      mult4flag = !mult4flag;
      lastMult4 = millis();
    }
    else{
      digitalWrite(MULT4, HIGH);
       mult4flag = !mult4flag;
       lastMult4 = millis();
    }
  } 
}
