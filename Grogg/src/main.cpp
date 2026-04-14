#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define rotCLK1 2
#define rotDT1 3
#define rotSW1 4

#define scrnSDA A5
#define scrnSCK A4

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int counter = 0;
int lastStateCLK = HIGH;
int currentStateCLK;
int MAXDRYCK1 = 30; // max 30 cl

// put function declarations here:
void updateDisplay(int screen, int counter) {
  display.setCursor(12,28);
  display.clearDisplay();
  display.println(String(counter) + " cl");
  display.display();
}

void displayPrintCl() {
  
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
  display.setCursor(16,28);
  display.println(String(counter) + " cl");
  display.display();
}

void loop() {
  currentStateCLK = digitalRead(rotCLK1);
  if (currentStateCLK != lastStateCLK) {
    if (digitalRead(rotDT1) != currentStateCLK && counter < MAXDRYCK1) {
      //MEDURS ROTATION
      counter++;
    } else if (counter > 0 && digitalRead(rotDT1) == currentStateCLK) {
      //MOTURS ROTATION
      counter--;
    }
    Serial.println(counter);
    updateDisplay(0, counter);
  }
  lastStateCLK = currentStateCLK;
  // put your main code here, to run repeatedly:
  
 
}