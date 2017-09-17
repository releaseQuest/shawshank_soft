//   {"lock":"stat"}
//   {"lock":"fire"}
//   {"lock":"open"}
//   {"lock":"arm"}
//   {"lock":"batt"}
//   {"lock":"gun"}

//  {"lock":"opened"}
//  {"lock":"trggrd"}
//  {"lock":"armed"}

// GND - blk
// Vcc - red
// Rx - grn
// Tx - wht

// blk - grn - wht

#include "IRremote.h"
#include "ArduinoJson.h"
#include "Servo.h"

#define OPEN 115
#define CLOSE 90

#define irIn 11
#define srv 5
#define rf_cmd 4
#define batt A3

IRrecv irrecv(irIn);
decode_results results;
Servo servo;

String inputString = "";
bool stringComplete = false;
char* verb;
String stat = "opened";
unsigned int gunCharge = 0;
bool isOpen = true;

void setup() {
  pinMode(rf_cmd, OUTPUT);
  
  servoMove( OPEN );

  digitalWrite(rf_cmd, LOW);
  Serial.begin(9600);
  Serial.println("AT+B1200");
  delay(500);
  Serial.end();
  delay(500);
  Serial.begin(1200);
  delay(500);
  Serial.println("AT+C073");
  delay(300);
  Serial.println("AT+FU4");
  delay(300);
  digitalWrite(rf_cmd, HIGH);
  
  irrecv.enableIRIn(); // Start the receiver
  
}

void loop() {  
  if (irrecv.decode(&results)) {
    if(results.value <= 100){
      gunCharge = results.value;
    }
    if( String(stat) == "armed" ){
      fireLock(false);
    } else if(String(stat) != "trggrd"){
      if( isOpen ){
        closeLock(false);
        isOpen = false;
      } else {
        openLock(false);
        isOpen = true;      
      }
    }
    irrecv.resume();
  }
  if (stringComplete) {
    manage();
    inputString = "";
    stringComplete = false;
  }
}

void servoMove(int pos){
  servo.attach(srv);
  servo.write(pos);
  delay(300);
  servo.detach();  
}

void manage(){
  delay(1);
  StaticJsonBuffer<50> jsonBuffer;
  JsonObject & root = jsonBuffer.parseObject(inputString);
  if ( root.success() ) {
    verb = root["lock"];
    execVerb();
  } else {
    delay(2500);
    Serial.println("{\"lock\":\"error\"}");
  }
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      stringComplete = true;
    } else {
      inputString += inChar;
    }
  }
}

int getBatteryCharge(){
  return map( analogRead( batt ), 750, 1023, 0, 100 );
}

void openLock(bool response){
  servoMove(OPEN);
  stat = "opened";
  isOpen = true;
  if(response){
    delay(3000);
    Serial.println("{\"lock\":\"opened\"}");
  }
}

void closeLock(bool response) {
  servoMove(CLOSE);
  stat = "closed";
  isOpen = false;
  if(response){
    delay(3000);
    Serial.println("{\"lock\":\"closed\"}");
  }
}

void fireLock(bool response) {
  servoMove(OPEN);
  stat = "trggrd";
  isOpen = false;
  if(response){
    delay(3000);
  }
  Serial.println("{\"lock\":\"trggrd\"}");
}

void execVerb(){
    if (String(verb) == "stat") {
      delay(2500);
      Serial.println("{\"lock\":\"" + stat + "\"}");         
    } else if (String(verb) == "fire") {
      fireLock(true);
    } else if (String(verb) == "open") {
      openLock(true);
    }  else if (String(verb) == "arm") {
      servoMove(CLOSE);
      stat = "armed";
      delay(2500);
      Serial.println("{\"lock\":\"armed\"}");
    } else if (String(verb) == "batt") {
      delay(2500);
      Serial.println("{\"lock\":\"" + String (getBatteryCharge() ) + "\"}");         
    } else if (String(verb) == "gun") {
      delay(2500);
      Serial.println("{\"lock\":\"" + String (gunCharge) + "\"}");         
    }
}
