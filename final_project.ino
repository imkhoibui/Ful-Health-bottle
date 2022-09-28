#include <PCM.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "pitches.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin Declarations
// const int buttonUp = 6; // not working
const int buttonUp = 2;
const int buttonChange = 5;
const int buttonEnter = 3;

const int ledGreen = 9;
const int ledYellow = 10;
const int ledRed = 11;

const int buzzer = 12;

int currentBox;
long timers[3] = { -1, -1, -1};
boolean ended[3] = {false, false, false};
const boolean hasEnded[3] = {true, true, true};
boolean setDone = false;

// Music Notes
int melody[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {4, 8, 8, 4, 4, 4, 4, 4};

void setup() {
  /*
    This function sets up the pins, inputs & outputs for the arduino
  */
  Serial.begin(9600); // Serial to check i/o
  Wire.begin();
  // Set up for screen
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.clearDisplay();
  display.display();
  // Set up for buttons
  pinMode(buttonUp, INPUT_PULLUP);
  pinMode(buttonChange, INPUT_PULLUP);
  pinMode(buttonEnter, INPUT_PULLUP);
  // Set up for leds
  pinMode(ledGreen, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledRed, OUTPUT);
  // Set up for buzzer
  pinMode(buzzer, OUTPUT);
}

void welcomeScreen() {
  /* This function display the welcoming text to user
  */
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(5, 5);
  display.setTextSize(1);
  display.println("Welcome User!");
  display.setCursor(5, 20);
  display.setTextSize(1);
  display.println("DST_06");
  display.display();
}

void proceed() {
  /* This function asks for user input Enter and proceed with the box choice
  */
  while (!digitalRead(buttonEnter)) {
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setCursor(5, 5);
    display.setTextSize(1);
    display.println("Press Enter");
    display.setCursor(5, 20);
    display.println("to continue");
    display.display();
  }
}

int chooseBox(int boxPos) {
  /* This function chooses the box to set time
     @para boxPos - the position of the box we're setting
  */
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(5, 5);
  display.setTextSize(1);
  display.println("Choose a box");
  display.setCursor(5, 20);
  display.setTextSize(1);
  display.println(">A   B   C");
  display.display();
  while (!digitalRead(buttonEnter)) {
    if (digitalRead(buttonChange)) {
      delay(250);
      boxPos++;
      currentBox = displayBoxChoice(boxPos);
    }
  }
  return currentBox;
}

int displayBoxChoice(int boxPos) {
  /* This function displays the screen for user to select which box
     they want to set time
     @para boxPos - the position of the box we're setting
     @return the number indicating the position of the box
  */
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(5, 5);
  display.setTextSize(1);
  display.println("Choose a box");
  if (boxPos % 3 == 0) {
    display.setCursor(5, 20);
    display.println(">A   B   C");
    display.display();
    return 0;
  } else if (boxPos % 3 == 1) {
    display.setCursor(5, 20);
    display.println(" A  >B   C");
    display.display();
    return 1;
  } else if (boxPos % 3 == 2) {
    display.setCursor(5, 20);
    display.println(" A   B  >C");
    display.display();
    return 2;
  }
}

void timeSet(int setUpBox) {
  /* This function proceeds the box to set its time and
     save the value to timers array
     @para setUpBox - the box to set time
  */
  int hours = 0;
  int mins = 0;
  int pos = 0; // chẵn for hours, lẻ for mins
  displayTimer(hours, mins, pos);
  delay(250);
  while (!digitalRead(buttonEnter)) {
    // Setting time methods
    if ((digitalRead(buttonUp)) && (pos % 2 == 0)) {
      delay(150);
      if (hours == 59) {
        hours = -1;
      }
//      Serial.println(hours);
      hours++;
      displayTimer(hours, mins, pos);
    } else if ((digitalRead(buttonUp)) && (pos % 2 == 1)) {
      delay(150);
      if (mins == 59) {
        mins = -1;
      }
      mins++;
      displayTimer(hours, mins, pos);
    }
    if (digitalRead(buttonChange)) {
      delay(150);
      pos++;
      displayTimer(hours, mins, pos);
    }
    timers[setUpBox] = 60 * 60 * hours + 60 * mins; // set the time value for that specific box
    Serial.println("Time is: ");
    Serial.println(timers[setUpBox]);
  }
}

boolean isFinish(){
  boolean finish = false;
  int pos = 0;
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(5, 5);
  display.setTextSize(1);
  display.println("Continue setting?");
  display.setCursor(5, 20);
  display.println(">Yes    No");
  display.display();
  delay(200);
  while(!digitalRead(buttonEnter)){
//    Serial.println("It's working!");
    if(digitalRead(buttonChange) && (pos % 2 == 1)){
      delay(200);
      pos++;
//      Serial.println("Hey!!!");
      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setCursor(5, 5);
      display.setTextSize(1);
      display.println("Continue setting?");
      display.setCursor(5, 20);
      display.println(">Yes    No");
      display.display();
      finish = false;
    }
    else if(digitalRead(buttonChange) && (pos % 2 == 0)){
      delay(200);
      pos++;
//      Serial.println("Aloooo!!!");
      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setCursor(5, 5);
      display.setTextSize(1);
      display.println("Continue setting?");
      display.setCursor(5, 20);
      display.println(" Yes   >No");
      display.display();
      finish = true;
    }
  }
//  Serial.print(finish);
  return finish; // return False to continue, True to exit
}

void displayTimer(int hours, int mins, int pos) {
  /*
     This function displays the timer while setting
  */
  String displayTime = "";
  if (hours < 10) {
    displayTime = displayTime + 0 + hours + ":";
  } else if (hours > 10) {
    displayTime = displayTime + hours + ":";
  }
  if (mins < 10) {
    displayTime = displayTime + 0 + mins;
  } else if (mins > 10) {
    displayTime = displayTime + mins;
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(5, 5);
  display.setTextSize(1);
  if (pos % 2 == 0) {
    display.println("Set time: Hour");
  }
  else if (pos % 2 == 1) {
    display.println("Set time: Minute");
  }
  display.setCursor(5, 20);
  display.setTextSize(1);
  display.println(displayTime);
  display.display();
}

void displayTimeRemaining(int hours, int mins) {
  /*
     This function displays the time remaining
  */
  String displayTime = "";
  if (hours < 10) {
    displayTime = displayTime + 0 + hours + ":";
  } else if (hours > 10) {
    displayTime = displayTime + hours + ":";
  }
  if (mins < 10) {
    displayTime = displayTime + 0 + mins;
  } else if (mins > 10) {
    displayTime = displayTime + mins;
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(5, 5);
  display.setTextSize(1);
  display.println("Time Remaining");
  display.setCursor(5, 20);
  display.setTextSize(1);
  display.println(displayTime);
  display.display();
}

void startCountdown() {
  /*
     This function countdown
  */
  // if timer reaches 0, alarming
  long temp = 1000000;
  for (int i = 0; i < 3; i++) {
      if(timers[i] != -1 && timers[i] < temp){
        temp = timers[i];
      }
      else if(timers[i]){
        ended[i] = true;
      }
  }
  int hours = temp / 3600;
  int mins = (temp % 3600) / 60;
  displayTimeRemaining(hours, mins);
  int boxPos = -1;
  long timeValue = 1000000;
  while (ended != hasEnded) {
    delay(60000); // 1 minute passes
    for (int i = 0; i < 3; i++) {
      if(timers[i] != -1){
        timers[i] = timers[i] - 60;
      }
      if(timers[i] != -1 && timers[i] < timeValue){
        timeValue = timers[i];
      }
      Serial.println("Each value is: ");
      Serial.println(timers[i]);
      if (timers[i] == 0) {
        boxPos = i;
      }
    }
    int hrs = timeValue / 3600;
    int minutes = (timeValue % 3600) / 60;
    displayTimeRemaining(hrs, minutes);
    Serial.println("Time value is: ");
    Serial.println(timeValue);
    if (boxPos != -1){
      alarming(boxPos);
      timers[boxPos] = -1;
      ended[boxPos] = true;
      timeValue = 1000000;
      boxPos = -1;
    }
    
    
//    Serial.println("Time Value is: ");
//    Serial.println(timeValue);
//    int hrs = timeValue / 3600;
//    int minutes = (timeValue % 3600) / 60;
//    Serial.println("Minutes: ");
//    Serial.println(minutes);
//    displayTimeRemaining(hrs, minutes);
  }
}

void alarming(int boxPos) {
  /*
     This function alarms the user which box to take out
  */
  if (boxPos == 0) {
    digitalWrite(ledGreen, HIGH);
  }
  else if (boxPos == 1) {
    digitalWrite(ledRed, HIGH);
  }
  else if (boxPos == 2) {
    digitalWrite(ledYellow, HIGH);
  }
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(buzzer, melody[thisNote], noteDuration);
    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(buzzer);
  }
  delay(10000);
  if (boxPos == 0) {
    digitalWrite(ledGreen, LOW);
  }
  else if (boxPos == 1) {
    digitalWrite(ledRed, LOW);
  }
  else if (boxPos == 2) {
    digitalWrite(ledYellow, LOW);
  }
}

void loop() {
  /*
     Main operation function
  */
  // Screen appearing, saying "welcome user, dst06"
  welcomeScreen();
  delay(5000);
  display.clearDisplay();

  // Enter to continue screen
  proceed();
  delay(500);

  // After pressing "Enter", screen ask the user to choose a box

  while (!setDone) {
    int setUpBox = chooseBox(0);
    delay(500);
    timeSet(setUpBox);
    delay(500);
    setDone = isFinish();
    delay(500);
  }
  Serial.println("Out loop");
  //if the user finishes setting up the product
  startCountdown();
  delay(1000);
}
