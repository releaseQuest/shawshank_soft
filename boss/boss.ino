
//   {"boss":"status"}

//   {"gate":"status"} 
//   {"gate":"open"}
//   {"gate":"close"}
//   {"gate":"fire"}

//   {"fan":"status"} 
//   {"fan":"start"}
//   {"fan":"stop"}

//   {"door":"status"}
//   {"door":"voice"}

//   {"vynyl":"status"}

//   {"led_off":2}

//   {"switches":"status"}



#include "ArduinoJson.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include "SimpleTimer.h"


#define rx_tx 2

#define btn_0 7
#define btn_1 8
#define btn_2 9
#define btn_3 6

#define lmp_0 A0
#define lmp_1 A1
#define lmp_2 A2
#define lmp_3 A3

#define stp 4
#define dir 5

#define rel_0 A4
#define rel_1 A5

#define s_rx 10
#define s_tx 12

#define vnl 3
#define powerOut 11

SoftwareSerial mp3(s_rx, s_tx);
DFRobotDFPlayerMini myDFPlayer;

SimpleTimer mainLightsTimer;

String gateStatus = "opened";
String fanStatus = "stoped";
String doorStatus = "not-played";
String vynylStatus = "off";
int switchesStatus [4] = {0, 0, 0, 0};

bool isRunning = false;
bool stringComplete = false;
String inputString = "";
char* verb;
bool vnlFlag = true;

int power = 0;
int charCounter = 0;

bool permToOpenGate = false;
bool mainLightsFree = true;
bool afterMainLights = false;
bool canSwitch = true;
bool guardsBusy = false;

int timerId = 0;

void setup() {
  delay(1000);
  pinMode(rx_tx, OUTPUT);
  pinMode(powerOut, OUTPUT);

  pinMode(lmp_0, OUTPUT);
  pinMode(lmp_1, OUTPUT);
  pinMode(lmp_2, OUTPUT);
  pinMode(lmp_3, OUTPUT);

  pinMode(rel_0, OUTPUT);
  pinMode(rel_1, OUTPUT);

  pinMode(stp, OUTPUT);
  pinMode(dir, OUTPUT);

  pinMode(btn_0, INPUT_PULLUP);
  pinMode(btn_1, INPUT_PULLUP);
  pinMode(btn_2, INPUT_PULLUP);
  pinMode(btn_3, INPUT_PULLUP);
  
  pinMode(vnl, INPUT);

  digitalWrite(rel_0, LOW);
  digitalWrite(rel_1, LOW);
  
  digitalWrite(stp, LOW);
  digitalWrite(dir, LOW);
  
  inputString.reserve(100);
  

  Serial.begin(9600);
  digitalWrite(rx_tx, LOW);
  delay(500);

 
  mp3.begin(9600);
  
  myDFPlayer.begin(mp3);
  myDFPlayer.setTimeOut(500);
  myDFPlayer.volume(30);
  myDFPlayer.EQ(DFPLAYER_EQ_BASS);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
}

void loop() {
  if( !mainLightsFree ){
    mainLightsTimer.run();
  }
  
  if( digitalRead(vnl) && vnlFlag) {
    delay(500);
    digitalWrite(rx_tx, HIGH);
    delay(50);
    if( digitalRead(vnl) ) {
      guardsBusy = true;
      Serial.print("{\"vynyl\":\"on\"}\n");
      Serial.flush();
      vynylStatus = "on";
      vnlFlag = false;
    }
    delay(50);
    digitalWrite(rx_tx, LOW);
    delay(50);
  } else if( !digitalRead(vnl) && !vnlFlag) {
    delay(50);
    digitalWrite(rx_tx, HIGH);
    delay(50);
    Serial.print("{\"vynyl\":\"off\"}\n");
    Serial.flush();
    delay(50);
    digitalWrite(rx_tx, LOW);
    delay(50);
    vynylStatus = "off";
    vnlFlag = true;
  }
  

  if (!digitalRead(btn_0) != switchesStatus[0] && canSwitch){
    delay(100);
    if(!digitalRead(btn_0) != switchesStatus[0]){
      digitalWrite(rx_tx, HIGH);
      delay(50);
      switchesStatus[0] = (int) !digitalRead(btn_0);
      digitalWrite(lmp_0, switchesStatus[0]);
      checkDoor();
      String stat = "[" + String(switchesStatus[0] ? 2:1) + "," + String(switchesStatus[1] && !afterMainLights ? 2:1) + "," + String(switchesStatus[2] ? 2:1) + "," + String(switchesStatus[3] ? 2:1) + "]";
      powerChange( switchesStatus[0] );
      Serial.print("{\"switches\":" + stat + "}\n");
      Serial.flush();
      delay(50);
      digitalWrite(rx_tx, LOW);
      delay(50);
    }
  } else if (!digitalRead(btn_1) != switchesStatus[1] && (mainLightsFree || guardsBusy) && canSwitch){
    delay(100);
    if(!digitalRead(btn_1) != switchesStatus[1]){
      
      if(mainLightsFree || guardsBusy) {
        switchesStatus[1] = (int) !digitalRead(btn_1);
        digitalWrite(lmp_1, switchesStatus[1]);
        checkDoor();
        if(afterMainLights && digitalRead(btn_1)){
          afterMainLights = false;
        } else {
          if( !digitalRead(btn_1) ){
            mainLightsFree = false;
            if( !guardsBusy ){
              timerId = mainLightsTimer.setTimeout( 18000, turnOffMainLights);
            }
          }
          digitalWrite(rx_tx, HIGH);
          delay(50);
          String stat = "[" + String(switchesStatus[0] ? 2:1) + "," + String(switchesStatus[1] ? 2:1) + "," + String(switchesStatus[2] ? 2:1) + "," + String(switchesStatus[3] ? 2:1) + "]";
          powerChange( switchesStatus[1] );
          Serial.print("{\"switches\":" + stat + "}\n");
          Serial.flush(); 
          delay(50);
          digitalWrite(rx_tx, LOW);
          delay(50);
          
        }
      }

      
    }
  } else if (!digitalRead(btn_2) != switchesStatus[2] && canSwitch){
    delay(100);
    if(!digitalRead(btn_2) != switchesStatus[2]){
      digitalWrite(rx_tx, HIGH);
      delay(50);
      switchesStatus[2] = (int) !digitalRead(btn_2);
      digitalWrite(lmp_2, switchesStatus[2]);
      checkDoor();
      String stat = "[" + String(switchesStatus[0] ? 2:1) + "," + String(switchesStatus[1] && !afterMainLights ? 2:1) + "," + String(switchesStatus[2] ? 2:1) + "," + String(switchesStatus[3] ? 2:1) + "]";
      powerChange( switchesStatus[2] );
      Serial.print("{\"switches\":" + stat + "}\n");
      Serial.flush(); 
      delay(50);
      digitalWrite(rx_tx, LOW);
      delay(50);
    }
  } else if (!digitalRead(btn_3) != switchesStatus[3] && canSwitch){
    delay(100);
    if(!digitalRead(btn_3) != switchesStatus[3]){
      digitalWrite(rx_tx, HIGH);
      delay(50);
      switchesStatus[3] = (int) !digitalRead(btn_3);
      digitalWrite(lmp_3, switchesStatus[3]);
      checkDoor();
      String stat = "[" + String(switchesStatus[0] ? 2:1) + "," + String(switchesStatus[1] && !afterMainLights ? 2:1) + "," + String(switchesStatus[2] ? 2:1) + "," + String(switchesStatus[3] ? 2:1) + "]";
      powerChange( switchesStatus[3] );
      Serial.print("{\"switches\":" + stat + "}\n");
      Serial.flush(); 
      delay(50);
      digitalWrite(rx_tx, LOW);
      delay(50);
    }
  }
  
  if (stringComplete) {
    manage();
    inputString = "";
    stringComplete = false;
  }
  
}

void manage () {
  DynamicJsonBuffer  jsonBuffer;
  JsonObject & root = jsonBuffer.parseObject(inputString);
  if (root.success()) {
    verb = root["boss"];
    if(verb){
      delay(50);
      digitalWrite(rx_tx, HIGH);
      delay(50);
      if( String(verb) == "status"){
        Serial.print("{\"boss\":\"OK\"}\n");
        Serial.flush();
      }
      if( String(verb) == "shutdown"){
        shutDown();
        Serial.print("{\"boss\":\"OK\"}\n");
        Serial.flush();
      }
      delay(50);
      digitalWrite(rx_tx, LOW);
      delay(50);
      verb = "";
      return;      
    }

    verb = root["gate"];
    if(verb){
      delay(50);
      digitalWrite(rx_tx, HIGH);
      delay(50);
      if( String(verb) == "status"){
        Serial.print("{\"gate\":\"" + gateStatus + "\"}\n");
      } else if( String(verb) == "open"){
        openGate();
        Serial.print("{\"gate\":\"opened\"}\n");
      } else if( String(verb) == "close"){
        closeGate();
        Serial.print("{\"gate\":\"closed\"}\n");      
      } else if( String(verb) == "fire"){
        fireGate();
        Serial.print("{\"gate\":\"triggered\"}\n");
      }
      Serial.flush();
      delay(50);
      digitalWrite(rx_tx, LOW);
      delay(50);
      verb = "";
      return;      
    }

    verb = root["fan"];
    if(verb){
      delay(50);
      digitalWrite(rx_tx, HIGH);
      delay(50);
      if( String(verb) == "status"){
        Serial.print("{\"fan\":\"" + fanStatus + "\"}\n");      
      } else if( String(verb) == "start"){
        startFan();
        Serial.print("{\"fan\":\"started\"}\n");
      } else if( String(verb) == "stop"){
        stopFan();
        Serial.print("{\"fan\":\"stopped\"}\n");
      }
      Serial.flush();
      delay(50);
      digitalWrite(rx_tx, LOW);
      delay(50);
      verb = "";
      return;      
    }

    verb = root["door"];
    if(verb){
      delay(50);
      digitalWrite(rx_tx, HIGH);
      delay(50);
      if( String(verb) == "status"){
        Serial.print("{\"door\":\"" + doorStatus + "\"}\n");
      } else if( String(verb) == "voice"){
        permToOpenGate = true;
        voiceDoor();
        Serial.print("{\"door\":\"played\"}\n");
      } else if( String(verb) == "stop"){
        doorStatus = "stopped";
        myDFPlayer.pause();
        Serial.print("{\"door\":\"stoped\"}\n");
      }
      Serial.flush();
      delay(50);
      digitalWrite(rx_tx, LOW);
      delay(50);
      verb = "";
      return;      
    }

    verb = root["vynyl"];
    if(verb){
      delay(50);
      digitalWrite(rx_tx, HIGH);
      delay(50);
      if( String(verb) == "status"){
        Serial.print("{\"vynyl\":\"" + vynylStatus + "\"}\n");          
      }
      Serial.flush();
      delay(50);
      digitalWrite(rx_tx, LOW);
      delay(50);
      verb = "";
      return;      
    }



//    int ledNumber = root["led_off"];
//    if(ledNumber >= 1 && ledNumber <= 4){
//      delay(50);
//      digitalWrite(rx_tx, HIGH);
//      delay(50);
//      ledOff(ledNumber);
//      powerChange(false);
//      Serial.print("{\"led_off\":\"off\"}\n");
//      Serial.flush();
//      delay(50);
//      digitalWrite(rx_tx, LOW);
//      delay(50);
//      verb = "";
//      return;      
//    }


    
    verb = root["switches"];
    if(verb){
      delay(50);
      digitalWrite(rx_tx, HIGH);
      delay(50);
      if( String(verb) == "status"){
        String stat = "[" + String(switchesStatus[0]) + "," + String(switchesStatus[1]) + "," + String(switchesStatus[2]) + "," + String(switchesStatus[3]) + "]";
        Serial.print("{\"switches\":" + stat + "}\n");  
      }
      Serial.flush(); 
      delay(50);
      digitalWrite(rx_tx, LOW);
      delay(50);
      verb = "";
      return;      
    }
  }
  delay(50);
  digitalWrite(rx_tx, LOW);
  delay(50);
}
//-----------------------gate--------------------------
void openGate(){
  digitalWrite(rel_0, HIGH);
  digitalWrite(rel_1, HIGH);
  gateStatus = "opened"; 
}

void closeGate(){
  digitalWrite(rel_0, LOW);
  digitalWrite(rel_1, LOW);
  gateStatus = "closed";
}

void fireGate(){
  digitalWrite(rel_0, HIGH);
  digitalWrite(rel_1, HIGH);
  gateStatus = "triggered"; 
}
//------------------------------------------------

//------------------------fan-----------------------
void startFan(){
  fanStatus = "started";
  digitalWrite(stp, HIGH);
  digitalWrite(dir, LOW);
}

void stopFan(){
  fanStatus = "stoped";
  digitalWrite(stp, LOW);
  digitalWrite(dir, LOW);
}
//--------------------------------------------------

//-----------------------door--------------------------
void voiceDoor(){
  doorStatus = "played";
  myDFPlayer.loop(1);
}

//------------------------------------------------
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

void powerChange(boolean increase) {
  if(increase) {
    power += 64;
    power = power > 255 ? 255 : power;
  } else {
    power -= 64;
    power = power < 0 ? 0 : power;
  }
  analogWrite(powerOut, power);
}


void ledOff(int ledNumber) {
  switch(ledNumber) {
    case 1:
      digitalWrite(lmp_0, LOW);
      break;
   
    case 2:
      digitalWrite(lmp_1, LOW);
      break;
        
    case 3:
      digitalWrite(lmp_2, LOW);
      break;  
    case 4:
      digitalWrite(lmp_3, LOW);
      break;    
  }
  return;
}

void checkDoor() {
  if (switchesStatus[0] && switchesStatus[1] && switchesStatus[2] && switchesStatus[3] && permToOpenGate) {
    delay(50);
    digitalWrite(rx_tx, HIGH);
    delay(50);    
    
    Serial.print("{\"gate\":\"triggered\"}\n");
    Serial.flush();
    shutDown();
    delay(50);
    digitalWrite(rx_tx, LOW);
    delay(50);
    fireGate();
  }
}

void turnOffMainLights() {
  mainLightsTimer.deleteTimer( timerId );
  ledOff(2);
  powerChange(false);
  mainLightsFree  = true;
  afterMainLights = true;
}

void shutDown() {
  stopFan();
   
  digitalWrite(lmp_0, LOW);
  powerChange(false);
  delay(500);
  digitalWrite(lmp_1, LOW);
  powerChange(false);
  delay(500);
  digitalWrite(lmp_2, LOW);
  powerChange(false);
  delay(500);
  digitalWrite(lmp_3, LOW);
  powerChange(false);
  delay(500);
  canSwitch = false;
}
