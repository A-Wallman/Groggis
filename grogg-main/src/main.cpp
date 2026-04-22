#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define RIGHT 0
#define UP 1
#define LEFT 2
#define DOWN 3
#define CENTER 5

#define ROT_CLK_A 2
#define ROT_DT_A 3
#define ROT_SW_A 4

#define PUMP_A 11

#define SLAVE_IN 10
#define SLAVE_OUT 9

#define MASTER_IN_A 8
#define MASTER_OUT_A 7
#define MASTER_IN_B 6
#define MASTER_OUT_B 5
#define MASTER_BUTTON 13


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
int slaveALastState = LOW;
int slaveBLastState = LOW;
int masterButtonCurrentState = LOW;
int masterButtonLastState = LOW;

const int debounceDelay = 50;
unsigned long lastDebounceTime = 0;



boolean isMaster;

boolean initialized = false;
boolean aInitialized = false;
boolean bInitialized = false;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


/*int rotPumpMap(int rotCLK) { //kartlägger skruvgrej X till pump X

  switch (rotCLK) {
    case ROT_CLK_A:
      return PUMP_IN_A;

    default: 
      return -1;
  }
} */

void displayAlignAndPrint(String text, int xAlign, int yAlign, int textSize) {
  int16_t x=0,y=0,x1,y1;
  uint16_t w,h;
  int cursX,cursY;


  display.setTextSize(textSize);

  display.getTextBounds(text.c_str(), x, y, &x1, &y1, &w, &h);

  switch (xAlign) {
    case LEFT:
      cursX = 0;
      break;
    case CENTER:
      cursX = (SCREEN_WIDTH - w) / 2;
      break;
    case RIGHT:
      cursX = SCREEN_WIDTH - w;
      break;
  }
  switch (yAlign) {
    case UP:
      cursY = 0;
      break;
    case CENTER:
      cursY = (SCREEN_HEIGHT - h) / 2;
      break;
    case DOWN:
      cursY = SCREEN_HEIGHT - h;
      break;
  }

  display.setCursor(cursX,cursY);
  display.println(text);
}

void refreshDisplay(int number) {
  display.clearDisplay();
  displayAlignAndPrint(DISPLAY_UNIT, RIGHT, DOWN, DISPLAY_TEXTSIZE);
  displayAlignAndPrint(String(number),LEFT,DOWN, DISPLAY_TEXTSIZE);
  display.display();
}

int pumpDurationClToMs(int value) {
  return value; //TBA: kalibrera med pumparna och se hur pumptid förhåller sig till volym i cl
}

void sendPulse(int pin) {
  digitalWrite(pin, HIGH);
  delay(5);
  digitalWrite(pin,LOW);
}

void firePump(int pump, int duration) {
  if (duration!=0) {
    digitalWrite(pump,HIGH);
    delay(duration);
    digitalWrite(pump,LOW);
  }
}


void signalReceived() {
  firePump(PUMP_A,counter_A);
}

void buttonPressed() { //för master
  Serial.println("button pressed");
  sendPulse(MASTER_OUT_A);
  sendPulse(MASTER_OUT_B);
  firePump(PUMP_A,counter_A);
}

void listener() { //för sub
  
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
    refreshDisplay(*counter);
    
  }
  
  *CLKlastState = CLKcurrentState;
  /*
  // Knapptryck
  int SWcurrentState = digitalRead(rotSW);
  if (SWcurrentState == LOW && *SWlastState == HIGH) {
    delay(20); // Debounce delay
    if (digitalRead(rotSW) == LOW) {
      buttonPressed();
    }
  }
  *SWlastState = SWcurrentState;*/
}



/*void masterHandshake() {

  displayAlignAndPrint("Väntar på A...",CENTER,CENTER,DISPLAY_TEXTSIZE);

  while(digitalRead(MASTER_IN_A==LOW)) {
    sendPulse(MASTER_OUT_A);
    delay(100);
  }
  display.clearDisplay();

  displayAlignAndPrint("Väntar på B...",CENTER,CENTER,DISPLAY_TEXTSIZE);
  while(digitalRead(MASTER_IN_B==LOW)) {
    sendPulse(MASTER_OUT_B);
    delay(100);
  }
  display.clearDisplay();

  sendPulse(MASTER_OUT_A);
  sendPulse(MASTER_OUT_B);
}*/

void masterInit() {
  isMaster = true;

  Serial.println("masterInit");

  pinMode(MASTER_IN_A,INPUT);
  pinMode(MASTER_OUT_A,OUTPUT);

  pinMode(MASTER_IN_B,INPUT);
  pinMode(MASTER_OUT_B,OUTPUT);

  pinMode(MASTER_BUTTON,INPUT);

}

void slaveInit() {
  isMaster = false;

  Serial.println("slaveInit");

  pinMode(SLAVE_IN,INPUT);
  pinMode(SLAVE_OUT,OUTPUT);


}



void setup() {
  Serial.begin(9600);

  // Initialize I2C bus
  Wire.begin();

  // vrid nr1
  pinMode(ROT_CLK_A, INPUT);
  pinMode(ROT_DT_A, INPUT);
  pinMode(ROT_SW_A, INPUT_PULLUP);

  //pump nr1
  digitalWrite(PUMP_A,LOW);
  pinMode(PUMP_A, OUTPUT);

  //skärm nr 1
  /*if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 at 0x3C failed, trying 0x3D"));
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    }
  }*/
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  Serial.println(F("SSD1306 initialized successfully"));
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(4);
  pinMode(SLAVE_IN,INPUT);
  if (digitalRead(SLAVE_IN)==HIGH) { //på master arduinon är SLAVE_IN kopplad till konstant etta, därmed är detta en master-arduino
    masterInit();
  } else { //arduinon är slave
    slaveInit();
  }

}

void loop() {

  if (isMaster) { //master loop

    if (!initialized) {
      displayAlignAndPrint("Initialiserar...",CENTER,UP,DISPLAY_TEXTSIZE-1);
      display.display();
      if (!aInitialized) {

        if (digitalRead(MASTER_IN_A)==LOW) {
          sendPulse(MASTER_OUT_A);
          delay(100);
          Serial.println("pingar A..");
        }
        if (digitalRead(MASTER_IN_A)==HIGH) {
          aInitialized=true;
          displayAlignAndPrint("A redo",CENTER,CENTER,DISPLAY_TEXTSIZE-1);
          display.display();
        }
      }

      if (!bInitialized) {

        if (digitalRead(MASTER_IN_B)==LOW) {
          sendPulse(MASTER_OUT_B);
          delay(100);
          Serial.println("pingar B..");
        }
        if (digitalRead(MASTER_IN_B)==HIGH) {
          bInitialized=true;
          displayAlignAndPrint("B redo",CENTER,DOWN,DISPLAY_TEXTSIZE-1);
          display.display();
        }
      }

      if (aInitialized && bInitialized) {
        initialized = true;
        display.clearDisplay();
        displayAlignAndPrint("Allt Redo!", CENTER,CENTER,DISPLAY_TEXTSIZE+1);
        display.display();
        delay(100);
        refreshDisplay(0);
      }

    }

    if (initialized) { //master only, intitalized

      if (digitalRead(MASTER_BUTTON)==HIGH && masterButtonLastState==LOW) {
        buttonPressed();
        masterButtonLastState = HIGH;
        delay(50);
      }
      if (digitalRead(MASTER_BUTTON)==LOW) {
        masterButtonLastState = LOW;
      }
    }
  }

  if (!isMaster) { //slave loop
    
    if (!initialized) {
      displayAlignAndPrint("Initialiserar...",CENTER,UP,DISPLAY_TEXTSIZE-1);
      display.display();
    }

    if (!initialized && digitalRead(SLAVE_IN) == HIGH) {
      initialized = true;
      digitalWrite(SLAVE_OUT,HIGH);
      masterLastState = HIGH;
      refreshDisplay(0);
      delay(50);
    }

    if (initialized) {
      //add listener functionality
      if (digitalRead(SLAVE_IN)==HIGH && masterLastState == LOW) {
        signalReceived();
        masterLastState = HIGH;
      }
      if (digitalRead(SLAVE_IN) == LOW) {
        masterLastState = LOW;
      }

    }

  }

  if (initialized) { //gemensamt för alla arduino
    

    rotaryHandler(ROT_CLK_A, ROT_DT_A, ROT_SW_A, MAX_VALUE_A,
    &counter_A, &CLKlastState_A, &SWlastState_A);
      
    
      
  }

}