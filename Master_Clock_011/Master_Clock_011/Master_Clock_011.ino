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

int clockTime;
bool mode = 1;

  bool flag = true;
  int time1, time2;
unsigned long D1 = 0, D2 = 0, D3 = 0, D4 = 0, M2 = 0, M3 = 0, M4 = 0, counter = 0, lastClock = 0;
bool D1F = 0, D2F = 0, D3F = 0, D4F = 0, M2F = 0, M3F = 0, M4F = 0;
void setup() {
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

}

void loop() {
  if (mode){
  clockTime = 1024 - (analogRead(POT)) + 16;
  }
  else{
    clockTime = tapTempo();
  }
  if (millis() - lastClock > (clockTime / 4)) {
    counter++;
    lastClock = millis();
  }
  if (millis() - D4 > (clockTime / 4)) {
    if (counter % 1 == 0) {
      if (D4F) {
        digitalWrite(DIV4, LOW);
        D4 = millis();
        D4F = !D4F;
      }
      else {
        digitalWrite(DIV4, HIGH);
        D4 = millis();
        D4F = !D4F;
      }
    }
    if (counter % 3 == 0) {
      if (D3F) {
        digitalWrite(DIV3, LOW);
        D3 = millis();
        D3F = !D3F;
      }
      else {
        digitalWrite(DIV3, HIGH);
        D3 = millis();
        D3F = !D3F;
      }
    }
    if (counter % 2 == 0) {
      if (D2F) {
        digitalWrite(DIV2, LOW);
        D2 = millis();
        D2F = !D2F;
      }
      else {
        digitalWrite(DIV2, HIGH);
        D2 = millis();
        D2F = !D2F;
      }
    }
    if (counter % 4 == 0) {
      if (D1F) {
        digitalWrite(DIV1, LOW);
        D1 = millis();
        D1F = !D1F;
      }
      else {
        digitalWrite(DIV1, HIGH);
        D1 = millis();
        D1F = !D1F;
      }
    }

    if (counter % 8 == 0) {
      if (M2F) {
        digitalWrite(MULT2, LOW);
        M2 = millis();
        M2F = !M2F;
      }
      else {
        digitalWrite(MULT2, HIGH);
        M2 = millis();
        M2F = !M2F;
      }
    }

    if (counter % 12 == 0) {
      if (M3F) {
        digitalWrite(MULT3, LOW);
        M3 = millis();
        M3F = !M3F;
      }
      else {
        digitalWrite(MULT3, HIGH);
        M3 = millis();
        M3F = !M3F;
      }
    }

    if (counter % 16 == 0) {
      if (M4F) {
        digitalWrite(MULT4, LOW);
        M4 = millis();
        M4F = !M4F;
      }
      else {
        digitalWrite(MULT4, HIGH);
        M4 = millis();
        M4F = !M4F;
      }
    }
  }
}

int tapTempo(){
  if(digitalRead(BUTTON) == LOW && flag){
    time1 = millis();
    flag = !flag;
    delay(20);
  }
  else if(digitalRead(BUTTON) == LOW){
    time2 = millis();
    flag = !flag;
    delay(20);
  }
  return abs(time1-time2);
}

