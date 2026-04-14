#include <Arduino.h>

#define rotCLK1 2
#define rotDT1 3
#define rotSW1 4

int counter = 0;
int lastStateCLK = HIGH;
int currentStateCLK;
int MAXDRYCK1 = 30; // max 30 cl

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  pinMode(rotCLK1, INPUT);
  pinMode(rotDT1, INPUT);
  pinMode(rotSW1, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("Setup complete");
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
    Serial.print("Counter: ");
    Serial.println(counter);
  }
  lastStateCLK = currentStateCLK;
  // put your main code here, to run repeatedly:
  
 
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}