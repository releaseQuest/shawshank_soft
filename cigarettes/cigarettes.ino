//   {"cigarettes":"status"}
//   {"cigarettes":"fire"}
//   {"cigarettes":"open"}
//   {"cigarettes":"close"}

//  {"cigarettes":"armed"}
//  {"cigarettes":"opened"}
//  {"cigarettes":"closed"}
//  {"cigarettes":"triggered"}

#include "ArduinoJson.h"
#include "Servo.h"

// GND - blk
// Vcc - red
// Rx - grn
// Tx - wht

#define btn 2
#define servo_0 3 // right - door
#define rx_tx 4
#define servo_1 5 // left - trigger

#define DOOR_OPEN 120
#define DOOR_CLOSE 56
#define TRIGGER_ARMED 140
#define TRIGGER_FIRE 100

Servo servo0;
Servo servo1;

String inputString = "";
bool stringComplete = false;
char* cigarettes;
char* verb;
String stat = "armed";

int btnState = 0;
int charCounter = 0;

void setup() {
  pinMode(btn, INPUT_PULLUP);
  pinMode(rx_tx, OUTPUT);

  servo0.attach(servo_0); // right // door
  servo0.write(DOOR_OPEN);
  servo1.attach(servo_1); //left // trigger
  servo1.write(TRIGGER_FIRE);
  delay(500);
  servo0.detach();
  servo1.detach();
  
  inputString.reserve(200);
  delay(2000);
  Serial.begin(9600);
  delay(100);
  digitalWrite(rx_tx, LOW);
  delay(100);
  Serial.flush();
}

void loop() {
  if (!digitalRead(btn) && btnState == 0) {
    servo1.attach(servo_1);
    servo1.write(TRIGGER_ARMED);
    delay(500);
    servo1.detach();   
    btnState = 1;
  } else if (digitalRead(btn) && btnState == 1) {
    servo1.attach(servo_1);
    servo1.write(TRIGGER_FIRE);
    delay(500);
    servo1.detach();
    btnState = 0;
  }
  if (stringComplete) {
    manage();
    inputString = "";
    stringComplete = false;
  }
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (charCounter == 0 && inChar != '{' ){
        continue;
    } else if (inChar == '\n') {
      charCounter = 0;
      stringComplete = true;
    } else {
      charCounter ++;
      inputString += inChar;
    }
  }
}

void manage() {
  DynamicJsonBuffer  jsonBuffer;
  JsonObject & root = jsonBuffer.parseObject(inputString);
  if (root.success()) {
    verb = root["cigarettes"];
    if(verb){
      delay(50);
      digitalWrite(rx_tx, HIGH);
      delay(50);
      if (String(verb) == "status") {
        Serial.print("{\"cigarettes\":\"" + stat + "\"}\n");
        Serial.flush();
      } else if (String(verb) == "fire") {
        fire();
      } else if (String(verb) == "open") {
        door(true);
      } else if (String(verb) == "close") {
        door(false);
      }
    }
  }
  delay(50);
  digitalWrite(rx_tx, LOW);
  delay(50);
}


void fire() {
  servo0.attach(servo_0);
  delay(50);
  servo0.write(DOOR_OPEN);
  delay(1000);
  servo1.attach(servo_1);
  servo1.write(TRIGGER_FIRE);
  delay(500);
  servo1.detach();
  servo0.write(DOOR_CLOSE);
  Serial.print("{\"cigarettes\":\"triggered\"}\n");
  Serial.flush();
  stat = "triggered";
  delay(1000);
  servo0.detach();
}

void door(bool open) {
  servo0.attach(servo_0);
  delay(50);
  if (open) {
    servo0.write(DOOR_OPEN);
    delay(1000);
    Serial.print("{\"cigarettes\":\"opened\"}\n");
    Serial.flush();
    stat = "opened";
  } else {
    servo0.write(DOOR_CLOSE);
    delay(1000);
    Serial.print("{\"cigarettes\":\"closed\"}\n");
    Serial.flush();
    stat = "armed";
  }
  servo0.detach();
}
