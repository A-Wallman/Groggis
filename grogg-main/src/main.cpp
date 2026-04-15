#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define RIGHT 0
#define UP 1
#define LEFT 2
#define DOWN 3

#define rotCLK_A 2
#define rotDT_A 3
#define rotSW_A 4

#define rotCLK_B 5
#define rotDT_B 6
#define rotSW_B 7


//OANVÄNT
#define scrnSDA A5
#define scrnSCK A4

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define MAX_VALUE_A 10000

#define DISPLAY_TEXTSIZE 2
#define DISPLAY_UNIT "ms"
#define COUNTER_STEPSIZE 50


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int counterA = 0;
int lastStateCLK = HIGH;
int currentStateCLK;

// put function declarations here:


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

void refreshDisplay(int screen, int counter) {
  display.clearDisplay();
  displayAlignAndPrint(DISPLAY_UNIT, RIGHT, DOWN);
  displayAlignAndPrint(String(counter),LEFT,DOWN);
  display.display();
}

int pumpDurationClToMs(int value) {
  return value; //TBA: kalibrera med pumparna och se hur pumptid förhåller sig till volym i cl
}



void setup() {
  Serial.begin(9600);
  // vrid nr1
  pinMode(rotCLK_A, INPUT);
  pinMode(rotDT_A, INPUT);
  pinMode(rotSW_A, INPUT_PULLUP);

  pinMode(rotCLK_B,INPUT);
  pinMode(rotDT_B,INPUT);
  pinMode(rotSW_B,INPUT_PULLUP);


  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(DISPLAY_TEXTSIZE);
  displayAlignAndPrint(DISPLAY_UNIT,RIGHT,DOWN);
  displayAlignAndPrint(String(counter),LEFT,DOWN);
  display.display();
}

void loop() {
  currentStateCLK = digitalRead(rotCLK_A);
  if (currentStateCLK != lastStateCLK) {
    if (digitalRead(rotDT_A) != currentStateCLK && counter < MAX_VALUE_A) {
      //MEDURS ROTATION
      counterA+=COUNTER_STEPSIZE;
    } else if (counter > 0 && digitalRead(rotDT_A) == currentStateCLK) {
      //MOTURS ROTATION
      counterA-=COUNTER_STEPSIZE;
    }
    Serial.println(counter);
    refreshDisplay(0, counter);
  }
  lastStateCLK = currentStateCLK;


  // put your main code here, to run repeatedly:
  
 
}