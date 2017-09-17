// {"post":"status"}

// {"post":"open"}
// {"post":"close"}

// {"post":"fire"}

#include "ArduinoJson.h"
#include "AccelStepper.h"
#include "Servo.h"
#include "SimpleTimer.h"

#define   sendStep    6
#define   sendDir     3
#define   sendLimit   A1

#define   recStep     5
#define   recDir      2
#define   recLimit    A0

#define   recSensor1  A3
#define   recSensor2  A2

#define   sendGatePin 10
#define   recGatePin  9

#define   sendGateBlock A4
#define   recGateBlock  A5

#define   startBtn    11

#define   rxTx    12

AccelStepper sendMotor(AccelStepper::DRIVER, sendStep, sendDir);
AccelStepper recMotor(AccelStepper::DRIVER, recStep, recDir);

Servo sendGate;
Servo recGate;

SimpleTimer recTimer;

bool stringComplete = false;
String inputString = "";
int charCounter = 0;
char* verb;

const int ZERO_SPEED = 100;
const int ZERO_ACCEL = 200;
const int SEND_ZERO_STROKE = -2000;
const int REC_ZERO_STROKE = 2000;

const int REC_SPEED = 200;
const int REC_ACCEL = 100;
const int REC_STROKE = -700;
const int REC_DONE_STROKE = -500;

const int SEND_SPEED = 200;
const int SEND_ACCEL = 100;
const int SEND_STROKE = 700;
const int SEND_OPEN_STROKE = 500;

const int SEND_GATE_OPEN = 130;
const int SEND_GATE_CLOSE = 64;

const int REC_GATE_OPEN = 75;
const int REC_GATE_CLOSE = 133;

const int REC_DELAY = 30000;

bool sendFlag = false;
String postStatus = "open";

bool isProcessing = false;

void setup() {
  pinMode(sendLimit, INPUT);
  pinMode(recLimit, INPUT);
  
  pinMode(recSensor1, INPUT);
  pinMode(recSensor2, INPUT);
  
  pinMode(startBtn, INPUT_PULLUP);

  pinMode(rxTx, OUTPUT);

  pinMode(sendGateBlock, OUTPUT);
  delay(200);
  pinMode(recGateBlock, OUTPUT);
  delay(200);
  Serial.begin(9600); 
  digitalWrite(rxTx, LOW);
  delay(500);


  moveRecGate(REC_GATE_CLOSE);  
  moveSendGate(SEND_GATE_CLOSE);
  
  moveRecGate(REC_GATE_OPEN);  
  moveSendGate(SEND_GATE_OPEN);
  
  if(!digitalRead(sendLimit)) {
    resetSend();
  }
   if(!digitalRead(recLimit)) {
    resetRec();
 }
 openSendCarriagePos();
}

void loop() {
  if( !digitalRead(recSensor1) && digitalRead(startBtn) && !sendFlag) {    
    sendFlag = true;
    isProcessing = true;
    sendTube();
    if( !digitalRead(recSensor2) ) {
      recTimer.setTimeout( REC_DELAY, firePost);
    } else {
      openSendCarriagePos();
      sendFlag = false;
      isProcessing = false;
    }
  }

  if (stringComplete) {
    manage();
    inputString = "";
    stringComplete = false;
  }

  if (isProcessing) {
    recTimer.run();
  }
}

void manage () {
  DynamicJsonBuffer  jsonBuffer;
  JsonObject & root = jsonBuffer.parseObject(inputString);
  if (root.success()) {
    verb = root["post"];
    delay(50);
    digitalWrite(rxTx, HIGH);
    delay(50);
    if( String(verb) == "status"){
      Serial.print("{\"post\":\"" + postStatus + "\"}\n");
    }else if( String(verb) == "open"){
      openPost();
      Serial.print("{\"post\":\"opened\"}\n");
    }else if( String(verb) == "close"){
      closePost();
      Serial.print("{\"post\":\"closed\"}\n");
    }else if( String(verb) == "fire"){
      firePost();
      Serial.print("{\"post\":\"triggered\"}\n");
    }
    Serial.flush();
    delay(50);
    digitalWrite(rxTx, LOW);
    delay(50);
    verb = "";
    return; 
  }
}

bool resetSend () {
  sendMotor.setMaxSpeed(ZERO_SPEED);
  sendMotor.setAcceleration(ZERO_ACCEL);
  sendMotor.move(SEND_ZERO_STROKE);
  
  while(sendMotor.isRunning()){
    sendMotor.run();
    if( digitalRead(sendLimit) ){
      break;
    }
  }

  sendMotor.disableOutputs ();
  sendMotor.stop ();
  sendMotor.setCurrentPosition(0);

  return digitalRead(sendLimit);  
}

bool resetRec() {
  recMotor.setMaxSpeed(ZERO_SPEED);
  recMotor.setAcceleration(ZERO_ACCEL);
  recMotor.move(REC_ZERO_STROKE);
  
  while(recMotor.isRunning()){
    recMotor.run();
    if( digitalRead(recLimit) ){
      break;
    }
  }

  recMotor.disableOutputs ();
  recMotor.stop ();
  recMotor.setCurrentPosition(0);

  return digitalRead(recLimit);  
}

void recTube() {
  isProcessing = false;
  moveRecGate(REC_GATE_OPEN);
  
  recMotor.setMaxSpeed(REC_SPEED);
  recMotor.setAcceleration(REC_ACCEL);
  recMotor.moveTo(REC_STROKE);
  
  while(recMotor.isRunning()){
    recMotor.run();
  }

  recMotor.moveTo(REC_DONE_STROKE);
  
  while(recMotor.isRunning()){
    recMotor.run();
  }
  
  moveRecGate(REC_GATE_CLOSE);
  
}

void openSendCarriagePos() {

  sendMotor.setMaxSpeed(SEND_SPEED);
  sendMotor.setAcceleration(SEND_ACCEL);
  sendMotor.moveTo(SEND_OPEN_STROKE);
  
  while(sendMotor.isRunning()){
    sendMotor.run();
  }
  delay(500);
}

void sendTube() {
  moveSendGate(SEND_GATE_OPEN);
  sendMotor.setMaxSpeed(SEND_SPEED);
  sendMotor.setAcceleration(SEND_ACCEL);
  sendMotor.moveTo(SEND_STROKE);
  
  while(sendMotor.isRunning()){
    sendMotor.run();
  }
  delay(500);
  resetSend();
  moveSendGate(SEND_GATE_CLOSE);
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (charCounter == 0 && inChar != '{') {
      continue;
    } else if (inChar == '\n') {
      charCounter = 0;
      stringComplete = true;
    } else {
      inputString += inChar;
      charCounter++;
    }
  }
}

void moveRecGate( int pos ) {
  delay(200);
  digitalWrite(recGateBlock, HIGH);
  delay(200); 
  recGate.attach(recGatePin);
  recGate.write( pos );
  delay(500);
  recGate.detach();
  digitalWrite(recGateBlock, LOW);
  delay(200);  
}

void moveSendGate( int pos ) {
  delay(200); 
  digitalWrite(sendGateBlock, HIGH);
  delay(200); 
  sendGate.attach(sendGatePin);
  sendGate.write( pos );
  delay(500);
  sendGate.detach();
  digitalWrite(sendGateBlock, LOW); 
  delay(200); 
}

void openPost () {
  moveRecGate(REC_GATE_OPEN);
  moveSendGate(SEND_GATE_OPEN);
  openSendCarriagePos();
  postStatus = "opened";
}

void closePost () {
  moveRecGate(REC_GATE_CLOSE);
  moveSendGate(SEND_GATE_CLOSE);
  postStatus = "closed";
}

void firePost () {
  postStatus = "triggered";
  recTube();
}
