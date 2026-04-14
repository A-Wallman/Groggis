#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define RIGHT 0
#define UP 1
#define LEFT 2
#define DOWN 3

#define rotCLK1 2
#define rotDT1 3
#define rotSW1 4

#define scrnSDA A5
#define scrnSCK A4

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define MAX_VOLUME_1 30


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int counter = 0;
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
  displayAlignAndPrint("cl", RIGHT, DOWN);
  displayAlignAndPrint(String(counter),LEFT,DOWN);
  display.display();
}

int pumpDurationms(int value) {
  return value; //TBA: kalibrera med pumparna och se hur pumptid förhåller sig till volym i cl
}


void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(rotCLK1, INPUT);
  pinMode(rotDT1, INPUT);
  pinMode(rotSW1, INPUT_PULLUP);



  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(4);
  displayAlignAndPrint("cl",RIGHT,DOWN);
  displayAlignAndPrint(String(counter),LEFT,DOWN);
  display.display();
}

void loop() {
  currentStateCLK = digitalRead(rotCLK1);
  if (currentStateCLK != lastStateCLK) {
    if (digitalRead(rotDT1) != currentStateCLK && counter < MAX_VOLUME_1) {
      //MEDURS ROTATION
      counter++;
    } else if (counter > 0 && digitalRead(rotDT1) == currentStateCLK) {
      //MOTURS ROTATION
      counter--;
    }
    Serial.println(counter);
    refreshDisplay(0, counter);
  }
  lastStateCLK = currentStateCLK;
  // put your main code here, to run repeatedly:
  
 
}