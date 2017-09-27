//   {"tape":"status"}
//   {"tape":"fire"}
//   {"tape":"open"}
//   {"tape":"close"}
//   {"tape":"voice"}

//  {"tape":"armed"}
//  {"tape":"opened"}
//  {"tape":"closed"}
//  {"tape":"triggered"}

#include "ArduinoJson.h"
#include "Servo.h"
#include "SoftwareSerial.h"
#include "DFPlayer_Mini_Mp3.h"

// GND - blk
// Vcc - red
// Rx - grn
// Tx - wht

#define btn 2
#define relay 6
#define servo_0 3 // right - tray
#define rx_tx 4
#define servo_1 5 // left - trigger
#define servo_2 9 // bottom - door

#define TRAY_OPEN 100
#define TRAY_CLOSE 22
#define TRIGGER_ARMED 160
#define TRIGGER_FIRE 100
#define DOOR_OPEN 100
#define DOOR_CLOSE 22

Servo servo0;
Servo servo1;
Servo servo2;

SoftwareSerial df(8, 7); // RX, TX

String inputString = "";
bool stringComplete = false;
char* tape;
char* verb;
String stat = "armed";

int btnState = 0;
int charCounter = 0;

int soundCount = 1;
void setup() {
  pinMode(btn, INPUT_PULLUP);
  pinMode(rx_tx, OUTPUT);
  pinMode(relay, OUTPUT);

  servo0.attach(servo_0);
  servo0.write(TRAY_OPEN);
  servo1.attach(servo_1);
  servo1.write(TRIGGER_FIRE);
  servo2.attach(servo_2);
  servo2.write(DOOR_OPEN);
  delay(500);
  servo0.detach();
  servo1.detach();
  servo2.detach();

  inputString.reserve(200);
  Serial.begin(9600);
  digitalWrite(rx_tx, LOW);
  delay(500);

  df.begin(9600);
  mp3_set_serial (df);
  delay (500);
  mp3_set_volume (24);
  delay (100);
  digitalWrite(relay, LOW);
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
    verb = root["tape"];
    if (verb) {
      delay(50);
      digitalWrite(rx_tx, HIGH);
      delay(50);
      if (String(verb) == "status") {
        Serial.print("{\"tape\":\"" + stat + "\"}\n");
        Serial.flush();
      } else if (String(verb) == "fire") {
        fire();
      } else if (String(verb) == "open") {
        door(true);
      } else if (String(verb) == "close") {
        door(false);
      } else if (String(verb) == "voice1") {
        mp3_play(soundCount);
        soundCount++;
        if(soundCount > 10){
          soundCount = 1;  
        }
        delay(5000);
        Serial.print("{\"tape\":\"voice\"}\n");
        Serial.flush();
      } else if (String(verb) == "voice2") {
        mp3_play(21);
        delay(5000);
        Serial.print("{\"tape\":\"voice\"}\n");
        Serial.flush();
      }
    }
  }
  delay(50);
  digitalWrite(rx_tx, LOW);
  delay(50);
}


void fire() {
  servo2.attach(servo_2);
  servo2.write(DOOR_OPEN);
  delay(500);
  servo2.detach();
  servo0.attach(servo_0);
  delay(50);
  servo0.write(TRAY_CLOSE);
  delay(1500);
  servo1.attach(servo_1);
  servo1.write(TRIGGER_FIRE);
  delay(500);
  servo1.detach();
  servo0.write(TRAY_OPEN);
  Serial.print("{\"tape\":\"triggered\"}\n");
  Serial.flush();
  stat = "triggered";
  delay(1500);
  servo0.detach();
  servo2.attach(servo_2);
  servo2.write(DOOR_CLOSE);
  delay(500);
  servo2.detach();
}

void door(bool open) {
  delay(50);
  if (open) {
    digitalWrite(relay, LOW);
    servo2.attach(servo_2);
    servo2.write(DOOR_OPEN);
    delay(500);
    servo2.detach();
    Serial.print("{\"tape\":\"opened\"}\n");
    Serial.flush();
    stat = "opened";
  } else {
    digitalWrite(relay, HIGH);
    servo2.attach(servo_2);
    servo2.write(DOOR_CLOSE);
    delay(500);
    servo2.detach();
    Serial.print("{\"tape\":\"closed\"}\n");
    Serial.flush();
    stat = "closed";
  }
}

