#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define RIGHT 0
#define UP 1
#define LEFT 2
#define DOWN 3

#define ROT_CLK_A 2
#define ROT_DT_A 3
#define ROT_SW_A 4

#define PUMP_IN_A 10

#define SUB_A 8
#define SUB_B 9

#define MASTER 7

//OANVÄNT
#define SCRN_SDA_A A5
#define SCRN_SCK_A A4

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define MAX_VALUE_A 10000

#define DISPLAY_TEXTSIZE 2
#define DISPLAY_UNIT "ms"
#define COUNTER_STEPSIZE 50


int counter_A = 0;
int CLKlastState_A = HIGH;
int SWlastState_A = HIGH;
int masterLastState = LOW;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


/*int rotPumpMap(int rotCLK) { //kartlägger skruvgrej X till pump X

  switch (rotCLK) {
    case ROT_CLK_A:
      return PUMP_IN_A;

    default: 
      return -1;
  }
} */

void displayAlignAndPrint(String text, int xAlign, int yAlign) {
  int16_t x=0,y=0,x1,y1;
  uint16_t w,h;
  int cursX,cursY;

  display.getTextBounds(text.c_str(), x, y, &x1, &y1, &w, &h);

  switch (xAlign) {
    case LEFT:
      cursX =0;
      break;
    case RIGHT:
      cursX=SCREEN_WIDTH-w;
      break;
  }
  switch (yAlign) {
    case UP:
      cursY=0;
      break;
    case DOWN:
      cursY=SCREEN_HEIGHT-h;
      break;
  }

  display.setCursor(cursX,cursY);
  display.println(text);
}

void refreshDisplay(int screen, int number) {
  display.clearDisplay();
  displayAlignAndPrint(DISPLAY_UNIT, RIGHT, DOWN);
  displayAlignAndPrint(String(number),LEFT,DOWN);
  display.display();
}

int pumpDurationClToMs(int value) {
  return value; //TBA: kalibrera med pumparna och se hur pumptid förhåller sig till volym i cl
}


void firePump(int pumpIn, int duration) {
  if (duration!=0) {
    digitalWrite(pumpIn,HIGH);
    delay(duration);
    digitalWrite(pumpIn,LOW);
  }
}

void signalOut(int pin) { //för master
  digitalWrite(pin,HIGH);
  delay(100);
  digitalWrite(pin,LOW);
}

void signalReceived() {
  firePump(PUMP_IN_A,counter_A);
}

void buttonPressed() { //för master
  signalOut(SUB_A);
  signalOut(SUB_B);
  signalReceived();
}

void listener() { //för sub
  int masterCurrentState = digitalRead(MASTER);
  if (masterCurrentState == HIGH && masterLastState == LOW) {
    signalReceived();
  }
}

void rotaryHandler(int rotCLK, int rotDT, int rotSW, int maxValue,
  int* counter, int* CLKlastState, int* SWlastState) {
    
  int CLKcurrentState = digitalRead(rotCLK);
  if (CLKcurrentState != *CLKlastState) {

    if (digitalRead(rotDT) != CLKcurrentState && *counter < maxValue) {
      //MEDURS ROTATION
      *counter+=COUNTER_STEPSIZE;
    } else if (digitalRead(rotDT) == CLKcurrentState && *counter > 0) {
      //MOTURS ROTATION
      *counter-=COUNTER_STEPSIZE;
    }
    // Debounce delay?
    delay(5); 

    Serial.println(*counter);
    refreshDisplay(0, *counter);
    
  }
  
  *CLKlastState = CLKcurrentState;
  
  // Knapptryck
  int SWcurrentState = digitalRead(rotSW);
  if (SWcurrentState == LOW && *SWlastState == HIGH) {
    delay(20); // Debounce delay
    if (digitalRead(rotSW) == LOW) {
      buttonPressed();
    }
  }
  *SWlastState = SWcurrentState;
}


void setup() {
  Serial.begin(9600);

  // vrid nr1
  pinMode(ROT_CLK_A, INPUT);
  pinMode(ROT_DT_A, INPUT);
  pinMode(ROT_SW_A, INPUT_PULLUP);

  //pump nr1
  digitalWrite(PUMP_IN_A,LOW);
  pinMode(PUMP_IN_A, OUTPUT);

  //skärm nr 1
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(DISPLAY_TEXTSIZE);
  displayAlignAndPrint(DISPLAY_UNIT,RIGHT,DOWN);
  displayAlignAndPrint(String(counter_A),LEFT,DOWN);
  display.display();

  //skicka ut till sub-arduinos, dessa är tomma på själva sub-arduinos
  digitalWrite(SUB_A,LOW);
  pinMode(SUB_A,OUTPUT);
  digitalWrite(SUB_B,LOW);
  pinMode(SUB_A,OUTPUT);

  //lyssnar på master-arduino, den här är tom på master-arduino
  pinMode(MASTER,INPUT);

}

void loop() {
  rotaryHandler(ROT_CLK_A, ROT_DT_A, ROT_SW_A, MAX_VALUE_A,
    &counter_A, &CLKlastState_A, &SWlastState_A);

}