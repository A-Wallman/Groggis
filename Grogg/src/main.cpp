#include <Arduino.h>

#define rotCLK1 DD2
#define rotDT1 DD3
#define rotSW1 DD4

int counter = 0;
int lastStateCLK = HIGH;
int currentStateCLK;

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  pinMode(rotCLK1, INPUT);
  pinMode(rotDT1, INPUT);
  pinMode(rotSW1, INPUT_PULLUP);
}

void loop() {
  currentStateCLK = digitalRead(rotCLK1);
  if (currentStateCLK != lastStateCLK) {
    if (digitalRead(rotDT1) != currentStateCLK) {
      //MEDURS ROTATION
      counter++;
    } else {
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