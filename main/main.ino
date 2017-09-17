#include "ArduinoJson.h"
#include "SoftwareSerial.h"
#include "DFPlayer_Mini_Mp3.h"
#include "SimpleTimer.h"

//Serial:
// - PC communication - hw - serial
// - boards communication - hw - serial1
// - 2x mp3 modules
// - RF module

#define pcRxTx    45  
#define trapsRxTx 3
#define rfCmd     2

//#define mp3_0_tx  6
//#define mp3_0_rx  7

#define mp3_1_tx  A8
#define mp3_1_rx  A9

#define knock_sensor 4
#define knock_analog A0

#define sound_sel 5

SoftwareSerial mp3_1(mp3_1_rx, mp3_1_tx);

SimpleTimer timer;
SimpleTimer pcWatchTimer;

bool pcStringComplete = false;
String pcInputString = "";

bool trapStringComplete = false;
String trapInputString = "";


bool rfStringComplete = false;
String rfInputString = "";

bool bossStringComplete = false;
String bossInputString = "";

bool isCigarettesManual = false;

String inputDebugBuf = "";

int switches;
char* bossVerb;
char* pcVerb;
char* rfVerb;

String lockCmd = "";

const int KNOCK_SAMPLE_LENGTH = 300; 
const int KNOCK_DURATION_WINDOW = 3000;
const int KNOCKS_FOR_TRIGGER = 2; 
const int KNOCK_TRESHOLD = 400;

int knockSampleLength = KNOCK_SAMPLE_LENGTH;
int knockDurationWindow = KNOCK_DURATION_WINDOW;
int currentKnocks = 0;
int knocks = 0;

bool knockSensorFlag = true;
int wallSoundTimerId;
int pcWatchTimerId;

bool swStatus[4] = {false, false, false, false};

int bossCharsCounter = 0;
int pcCharsCounter = 0;

//---------------------------------------------------------------------------------------
void setup() {
  pinMode(pcRxTx, OUTPUT);
  pinMode(trapsRxTx, OUTPUT);
  pinMode(rfCmd, OUTPUT);
  pinMode(sound_sel, OUTPUT);
  pinMode(knock_sensor, INPUT_PULLUP);
  
  Serial.begin(19200);     // pc
  Serial1.begin(9600);    // traps
  Serial2.begin(9600);    // rf
  while (Serial.available()) {
    Serial.read();
  }  
  pcInputString.reserve(200);
  trapInputString.reserve(200);
  rfInputString.reserve(200);

  mp3_1.begin(9600);
  
  Serial3.begin(9600);
  
  digitalWrite(trapsRxTx, LOW);
  digitalWrite(sound_sel, HIGH);
  digitalWrite(rfCmd, LOW);
  
  delay (100);
  Serial2.println("AT+B1200");
  delay(500);
  Serial2.end();
  delay(500);
  Serial2.begin(1200);
  delay(500);
  Serial2.println("AT+C073");
  delay(500);
  Serial2.println("AT+FU4");
  delay(500);
  digitalWrite(rfCmd, HIGH);
  
  
  digitalWrite(pcRxTx, HIGH);
  Serial.println("{\"status\":\"on air\"}");
  Serial.flush();
  delay(100);
  digitalWrite(pcRxTx, LOW);
  delay(1000);
  while (Serial1.available()) {
    Serial1.read();
  }
}

//---------------------------------------------------------------------------------------
void loop() {  
  if (pcStringComplete) {
    managePC();
    pcInputString = "";
    pcStringComplete = false;
  }
  if (bossStringComplete) {
    manageBoss();
    bossInputString = "";
    bossStringComplete = false;
  }
  if (rfStringComplete) {
    manageRF();
    rfInputString = "";
    rfStringComplete = false;
  }
  if( knockSensorFlag && !isCigarettesManual){
    knockSensorController();  
  }
  timer.run();
}

//----------------------------------managing communication-----------------------------------------------

void manageBoss() {
  DynamicJsonBuffer  jsonBuffer;
  JsonObject & root = jsonBuffer.parseObject(bossInputString);
  if (root.success()) {
    bossVerb = root["vynyl"];
    if( (String) bossVerb == "on" ) {
      delay(50);
      digitalWrite(pcRxTx, HIGH);
      delay(50);
      Serial.println("{\"doorKnocks\":\"on\"}");
      digitalWrite(sound_sel, LOW);
    } else if( (String) bossVerb == "off" ) {
      delay(50);
      digitalWrite(pcRxTx, HIGH);
      delay(50);
      Serial.println("{\"doorKnocks\":\"off\"}");
    } 
    bossVerb = root["gate"];
    if( (String) bossVerb == "triggered" ) {
      delay(50);
      digitalWrite(pcRxTx, HIGH);
      delay(50);
      Serial.println("{\"gate\":\"boom\"}");
      digitalWrite(sound_sel, HIGH);
    }
    // fan
    switches = root["switches"][0];
    if( switches == 2 && !swStatus[0]){
      delay(50);
      digitalWrite(trapsRxTx, HIGH);
      delay(50);
      Serial1.print("{\"fan\":\"start\"}\n");
      swStatus[0] = true;      
    } else if ( switches == 1 && swStatus[0] ) {
      delay(50);
      digitalWrite(trapsRxTx, HIGH);
      delay(50);
      Serial1.print("{\"fan\":\"stop\"}\n");
      swStatus[0] = false;  
    }

    //ceilsBlock lights
    switches = root["switches"][1];
    if( switches == 2 && !swStatus[1]){
      delay(50);
      digitalWrite(pcRxTx, HIGH);
      delay(50);
      Serial.println("{\"ceilsBlockLights\":\"on\"}");
      swStatus[1] = true;      
    } else if ( switches == 1 && swStatus[1] ) {
      delay(50);
      digitalWrite(pcRxTx, HIGH);
      delay(50);
      Serial.println("{\"ceilsBlockLights\":\"off\"}");
      swStatus[1] = false;  
    }

    //bossBlock lights
    switches = root["switches"][2];
    if( switches == 2 && !swStatus[2]){
      delay(50);
      digitalWrite(pcRxTx, HIGH);
      delay(50);
      Serial.println("{\"bossBlockLights\":\"on\"}");
      swStatus[2] = true;      
    } else if ( switches == 1 && swStatus[2] ) {
      delay(50);
      digitalWrite(pcRxTx, HIGH);
      delay(50);
      Serial.println("{\"bossBlockLights\":\"off\"}");
      swStatus[2] = false;  
    }

    //death chair
    switches = root["switches"][3];
    if( switches == 2 && !swStatus[3]){
      delay(50);
      digitalWrite(pcRxTx, HIGH);
      delay(50);
      Serial.println("{\"deathChair\":\"on\"}");
      swStatus[3] = true;      
    } else if ( switches == 1 && swStatus[3] ) {
      delay(50);
      digitalWrite(pcRxTx, HIGH);
      delay(50);
      Serial.println("{\"deathChair\":\"off\"}");
      swStatus[3] = false;  
    }
  }
  delay(50);
  digitalWrite(pcRxTx, LOW);
  digitalWrite(trapsRxTx, LOW);
  delay(50);
}

void manageRF(){
  DynamicJsonBuffer  jsonBuffer;
  JsonObject & root = jsonBuffer.parseObject(rfInputString);
  if (root.success()) {
    rfVerb = root["lock"];
    if((String) rfVerb == "trggrd"){
      delay(50);
      digitalWrite(pcRxTx, HIGH);
      delay(50);
      Serial.println("{\"lock\":\"triggered\"}");
      delay(50);
      digitalWrite(pcRxTx, LOW);
      delay(50);
    }
  }  
}

void managePC(){
  DynamicJsonBuffer  jsonBuffer;
  JsonObject & root = jsonBuffer.parseObject(pcInputString);
  if (root.success()) {
    pcVerb = root["cigarettes"];
    if(pcVerb){
      delay(50);
      digitalWrite(pcRxTx, HIGH);
      delay(100);
      if((String) pcVerb == "fire"){
        if( actCigarettes() ){
          Serial.println("{\"cigarettes\":\"triggered\"}");
        } else {
          Serial.println("{\"cigarettes\":\"error\"}");
        }
      } else if((String) pcVerb == "status"){
        if( statusCigarettes() ){
          Serial.println("{\"cigarettes\":\"OK\"}");
        } else {
          Serial.println("{\"cigarettes\":\"error\"}");
        }          
      } else if((String) pcVerb == "open"){
        if( openCigarettes() ){
          Serial.println("{\"cigarettes\":\"opened\"}");
        } else {
          Serial.println("{\"cigarettes\":\"error\"}");
        }          
      } else if((String) pcVerb == "close"){
        if( closeCigarettes() ){
          Serial.println("{\"cigarettes\":\"closed\"}");
        } else {
          Serial.println("{\"cigarettes\":\"error\"}");
        }        
      } else if((String) pcVerb == "manual"){
        isCigarettesManual = true;
        Serial.println("{\"cigarettes\":\"manual\"}");       
      } else if((String) pcVerb == "auto"){
        isCigarettesManual = false;
        Serial.println("{\"cigarettes\":\"auto\"}");         
      }  else if((String) pcVerb == "voice"){
        if( knockSensorFlag ){
          wallSoundTimerId = timer.setInterval(10000, wallVoice);
        }
        Serial.println("{\"cigarettes\":\"voice\"}");        
      }

      
      trapInputString = "";
      trapStringComplete = false;
      delay(50);
      digitalWrite(pcRxTx, LOW);
      delay(50);
      pcVerb = "";
      return;
    }

    
    pcVerb = root["tape"];
    if(pcVerb){
      delay(50);
      digitalWrite(pcRxTx, HIGH);
      delay(100);
      if( (String) pcVerb == "fire" ){
        if( fireTape() ){
          timer.deleteTimer( wallSoundTimerId );
          Serial.println("{\"tape\":\"triggered\"}");
        } else {
          Serial.println("{\"tape\":\"error\"}");
        }
      } else if((String) pcVerb == "status"){
        if( statusTape() ){
          Serial.println("{\"tape\":\"OK\"}");
        } else {
          Serial.println("{\"tape\":\"error\"}");
        }          
      } else if((String) pcVerb == "open"){
        if( openTape() ){
          Serial.println("{\"tape\":\"opened\"}");
        } else {
          Serial.println("{\"tape\":\"error\"}");
        }          
      } else if((String) pcVerb == "close"){
        if( closeTape() ){
          Serial.println("{\"tape\":\"closed\"}");
        } else {
          Serial.println("{\"tape\":\"error\"}");
        }        
      } else if((String) pcVerb == "voice1"){
        if( voiceTape(1) ){
          Serial.println("{\"tape\":\"voice\"}");
        } else {
          Serial.println("{\"tape\":\"error\"}");
        }        
      } else if((String) pcVerb == "voice2"){
        if( voiceTape(2) ){
          Serial.println("{\"tape\":\"voice\"}");
        } else {
          Serial.println("{\"tape\":\"error\"}");
        }        
      }
      trapInputString = "";
      trapStringComplete = false;
      delay(50);
      digitalWrite(pcRxTx, LOW);
      delay(50);
      pcVerb = "";
      return;
    }

  
    pcVerb = root["lock"];
    if(pcVerb){
      delay(50);
      digitalWrite(pcRxTx, HIGH);
      delay(100);
      if( (String) pcVerb == "fire" ){
        if ( fireLock() ){
          Serial.println("{\"lock\":\"triggered\"}");
        } else {
          Serial.println("{\"lock\":\"error\"}");
        } 
      } else if((String) pcVerb == "status"){
        if ( statusLock() ){
          Serial.println("{\"lock\":\"OK\"}");
        } else {
          Serial.println("{\"lock\":\"error\"}");
        } 
      } else if((String) pcVerb == "open"){
        if ( openLock() ){
          Serial.println("{\"lock\":\"opened\"}");
        } else {
          Serial.println("{\"lock\":\"error\"}");
        }         
      } else if((String) pcVerb == "arm"){
        if ( armLock() ){
          Serial.println("{\"lock\":\"armed\"}");
        } else {
          Serial.println("{\"lock\":\"error\"}");
        } 
      } else if((String) pcVerb == "battery"){
        int bat = batteryLock();
        if ( bat > -1 ){
          Serial.println("{\"lock\":\"" + String (bat) + "\"}");
        } else {
          Serial.println("{\"lock\":\"error\"}");
        }        
      } else if((String) pcVerb == "gun"){ 
        int bat = gunLock();
        if ( bat > -1 ){
          Serial.println("{\"lock\":\"" + String (bat) + "\"}");
        } else {
          Serial.println("{\"lock\":\"error\"}");
        }  
      }
      rfInputString = "";
      rfStringComplete = false;
      delay(50);
      digitalWrite(pcRxTx, LOW);
      delay(50);
      pcVerb = "";
      return;
    }
  
    pcVerb = root["gate"];
    if(pcVerb){
      delay(50);
      digitalWrite(pcRxTx, HIGH);
      delay(100);
      if( (String) pcVerb == "fire" ){
        if( fireGate() ){
          Serial.println("{\"gate\":\"triggered\"}");
        } else {
          
          Serial.println("{\"gate\":\"error\"}");
        }
      } else if((String) pcVerb == "status"){
        if( statusGate() ){
          Serial.println("{\"gate\":\"OK\"}");
        } else {
          
          Serial.println("{\"gate\":\"error\"}");
        }          
      } else if((String) pcVerb == "open"){
        if( openGate() ){
          Serial.println("{\"gate\":\"opened\"}");
        } else {
          
          Serial.println("{\"gate\":\"error\"}");
        }          
      } else if((String) pcVerb == "close"){
        if( closeGate() ){
          Serial.println("{\"gate\":\"closed\"}");
        } else {
        
          Serial.println("{\"gate\":\"error\"}");
        }        
      }
      trapInputString = "";
      trapStringComplete = false;
      delay(50);
      digitalWrite(pcRxTx, LOW);
      delay(50);
      pcVerb = "";
      return;
    }
    
   int led_number = root["led_off"];
    if(led_number){
      delay(50);
      digitalWrite(pcRxTx, HIGH);
      delay(100);
      
      swStatus[led_number-1] = false;
      Serial.println("{\"led_off\":\"off\"}");
     
      trapInputString = "";
      trapStringComplete = false;
      
      delay(50);
      digitalWrite(pcRxTx, LOW);
      delay(50);
      led_number = 0;
      
      return;
    }

   int help_number = root["help"];
    if(help_number){
      delay(50);
      digitalWrite(pcRxTx, HIGH);
      delay(100);
      
      attachMp3(1);
      mp3_play(help_number);

      Serial.println("{\"help\":\"played\"}");
     
      trapInputString = "";
      trapStringComplete = false;
      delay(50);
      digitalWrite(pcRxTx, LOW);
      delay(50);
      help_number = 0;
      return;
    }
    
    pcVerb = root["fan"];
    if(pcVerb){
      delay(50);
      digitalWrite(pcRxTx, HIGH);
      delay(100);
      if( (String) pcVerb == "start" ){
        if( startFan() ){
          Serial.println("{\"fan\":\"started\"}");
        } else {
          Serial.println("{\"fan\":\"error\"}");
        }
      } else if((String) pcVerb == "status"){
        if( statusFan() ){
          Serial.println("{\"fan\":\"OK\"}");
        } else {
          Serial.println("{\"fan\":\"error\"}");
        }          
      } else if((String) pcVerb == "stop"){
        if( stopFan() ){
          Serial.println("{\"fan\":\"stoped\"}");
        } else {
          Serial.println("{\"fan\":\"error\"}");
        }          
      } 
      trapInputString = "";
      trapStringComplete = false;
      delay(50);
      digitalWrite(pcRxTx, LOW);
      delay(50);
      pcVerb = "";
      return;
    }

    pcVerb = root["door"];
    if(pcVerb){
      delay(50);
      digitalWrite(pcRxTx, HIGH);
      delay(100);
      if( (String) pcVerb == "voice" ){
        if( voiceDoor() ){
          Serial.println("{\"door\":\"triggered\"}");
        } else {
          Serial.println("{\"door\":\"error\"}");
        }
      } else if((String) pcVerb == "status"){
        if( statusDoor() ){
          Serial.println("{\"door\":\"OK\"}");
        } else {
          Serial.println("{\"door\":\"error\"}");
        }          
      } else if((String) pcVerb == "stop"){
        if( stopDoor() ){
          Serial.println("{\"door\":\"OK\"}");
        } else {
          Serial.println("{\"door\":\"error\"}");
        }          
      } 
      trapInputString = "";
      trapStringComplete = false;
      delay(50);
      digitalWrite(pcRxTx, LOW);
      delay(50);
      pcVerb = "";
      return;
    }

    pcVerb = root["post"];
    if(pcVerb){
      delay(50);
      digitalWrite(pcRxTx, HIGH);
      delay(100);
      if( (String) pcVerb == "fire" ){
        if( firePost() ){
          Serial.println("{\"post\":\"triggered\"}");
        } else {
          Serial.println("{\"post\":\"error\"}");
        }
      } else if((String) pcVerb == "status"){
        if( statusPost() ){
          Serial.println("{\"post\":\"OK\"}");
        } else {
          Serial.println("{\"post\":\"error\"}");
        }          
      } else if((String) pcVerb == "open"){
        if( openPost() ){
          Serial.println("{\"post\":\"opened\"}");
        } else {
          Serial.println("{\"post\":\"error\"}");
        }          
      } else if((String) pcVerb == "close"){
        if( closePost() ){
          Serial.println("{\"post\":\"closed\"}");
        } else {
          Serial.println("{\"post\":\"error\"}");
        }        
      }
      trapInputString = "";
      trapStringComplete = false;
      delay(50);
      digitalWrite(pcRxTx, LOW);
      delay(50);
      pcVerb = "";
      return;
    }
    
    pcVerb = root["boss"];
    if(pcVerb){
      delay(50);
      digitalWrite(pcRxTx, HIGH);
      delay(100);
      if((String) pcVerb == "status"){
        if( statusBoss() ){
          Serial.println("{\"boss\":\"OK\"}");
        } else {
          Serial.println("{\"boss\":\"error\"}");
        }          
      }
      
      trapInputString = "";
      trapStringComplete = false;
      delay(50);
      digitalWrite(pcRxTx, LOW);
      delay(50);
      pcVerb = "";
      return;
    }
    
    pcVerb = root["status"];
    if( (String) pcVerb == "test" ){
      delay(50);
      digitalWrite(pcRxTx, HIGH);
      delay(100);
      Serial.println("{\"status\":\"OK\"}");
      trapInputString = "";
      trapStringComplete = false;
      delay(50);
      digitalWrite(pcRxTx, LOW);
      delay(50);
      pcVerb = "";
      return;
    }
  } 
    
    delay(50);
    digitalWrite(pcRxTx, LOW);
    delay(50);
    trapInputString = "";
    trapStringComplete = false;
    pcVerb = "";
}
//---------------------------------------------------------------------------------------


//-------------------------------------------------------mp3 stuff---------------------------------------------
void attachMp3(int index){
  switch(index){
    case 0:
    mp3_set_serial (Serial3);
    break;

    case 1:
    mp3_set_serial (mp3_1);
    break;
  }
  delay(50);
  mp3_set_volume (24);
  delay(100);
}

//---------------------------------------serial events--------------------------------
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      pcStringComplete = true;
    } else {
      pcInputString += inChar;
      delay(100);
    }
  }
}

void serialEvent1() {
  while (Serial1.available()) {
    char inChar = (char)Serial1.read();
    if (bossCharsCounter == 0 && inChar != '{' ){
        continue;
    } else if (inChar == '\n') {
      bossCharsCounter = 0;  
      bossStringComplete = true;
    } else {
      bossCharsCounter ++;
      bossInputString += inChar;
    }
  }
}

void serialEvent2() {
    while (Serial2.available()) {
      char inChar = (char)Serial2.read();
      if (inChar == '\n') {
        rfStringComplete = true;
      } else {
        rfInputString += inChar;
      }
    }
}
//---------------------Cigarettes-------------------------------
//  Statuses:
//  {"cigarettes":"armed"}
//  {"cigarettes":"opened"}
//  {"cigarettes":"closed"}
//  {"cigarettes":"triggered"}
bool fireCigarettes(){
//  {"cigarettes":"fire"}
  return  trapVerb("cigarettes", "fire", "triggered");
}
//-------------------------------------------------------------------
bool statusCigarettes(){
//  {"cigarettes":"status"}
  delay(50);
  digitalWrite(trapsRxTx, HIGH);
  delay(50);
  Serial1.print("{\"cigarettes\":\"status\"}\n");
  delay(50);
  digitalWrite(trapsRxTx, LOW);
  delay(50);
  int counter = 0;
  while(counter < 3000){
    while (Serial1.available()) {
      char inChar = (char)Serial1.read();
      if (inChar == '\n') {
        trapStringComplete = true;
      } else {
        trapInputString += inChar;
      }
    }
    if(trapStringComplete){
      char* stat;
      DynamicJsonBuffer  jsonBuffer;
      JsonObject & root = jsonBuffer.parseObject(trapInputString);
      if (root.success()) {
        stat = root["cigarettes"];
        if( (String) stat != "error"){
          return true;
        }
      }
      return false;
    }
    delay(1);
    counter ++;
  }
  return false;
}
//-------------------------------------------------------------------
bool openCigarettes(){
//   {"cigarettes":"open"}
  knockSensorFlag = true;
  return  trapVerb("cigarettes", "open", "opened");
}
//-------------------------------------------------------------------
bool closeCigarettes(){
//   {"cigarettes":"close"}
  return  trapVerb("cigarettes", "close", "closed");
}

//-----------------------Tape----------------------------------
bool  fireTape(){
  return  trapVerb("tape", "fire", "triggered");
}

bool  statusTape(){
  delay(50);
  digitalWrite(trapsRxTx, HIGH);
  delay(50);
  Serial1.print("{\"tape\":\"status\"}\n");
  delay(50);
  digitalWrite(trapsRxTx, LOW);
  int counter = 0;
  while(counter < 3000){
    while (Serial1.available()) {
      char inChar = (char)Serial1.read();
      if (inChar == '\n') {
        trapStringComplete = true;
      } else {
        trapInputString += inChar;
      }
    }
    if(trapStringComplete){
      char* stat;
      DynamicJsonBuffer  jsonBuffer;
      JsonObject & root = jsonBuffer.parseObject(trapInputString);
      if (root.success()) {
        stat = root["tape"];
        if( (String) stat != "error"){
          return true;
        }
      }
      return false;
    }
    delay(1);
    counter ++;
  }
  return false;
}

bool  openTape(){
  return  trapVerb("tape", "open", "opened");
}

bool  closeTape(){
  return  trapVerb("tape", "close", "closed");
}

bool  voiceTape(int number){
  switch(number){
    case 1: 
      return trapVerb("tape", "voice1", "voice");
    case 2:
      return trapVerb("tape", "voice2", "voice");
    default:
      return trapVerb("tape", "voice1", "voice");
  }  
}

//------------------------lock-------------------------------------------------------
// main - lock
//   {"lock":"stat"}
//   {"lock":"fire"}
//   {"lock":"open"}
//   {"lock":"arm"}
//   {"lock":"batt"}
//   {"lock":"gun"}

//  {"lock":"opened"}
//  {"lock":"trggrd"}
//  {"lock":"armed"}
//-----------------------------------------------------------------------------------
int fireLock(){
 return lockVerb("fire", "trggrd");
}

int statusLock(){
 return lockVerb("stat", "null");
}

int openLock(){
 return lockVerb("open", "opened");
}

int armLock(){
 return lockVerb("arm", "armed");
}

int batteryLock(){
  return lockVerb("batt", "null");
}

int gunLock(){
  return lockVerb("gun", "null");
}

//-------------------------------boss---------------------------------------------

bool statusBoss() {
  return bossStat("boss");
}

//-------------------------------gate----------------------------------------------

bool statusGate() {
  return bossStat("gate");
}

bool openGate() {
  return trapVerb("gate", "open", "opened");
}

bool closeGate() {
  return trapVerb("gate", "close", "closed");
}

bool fireGate() {
  return trapVerb("gate", "fire", "triggered");
}

//-------------------------------fan---------------------------------------------------

bool startFan() {
  return trapVerb("fan", "start", "started");
}

bool stopFan() {
  return trapVerb("fan", "stop", "stopped");
}

bool statusFan() {
  return bossStat("fan");
}

//----------------------------door------------------------------------------------------

bool voiceDoor() {
  return trapVerb("door", "voice", "played");
}

bool statusDoor() {
  return bossStat("door");
}

bool stopDoor() {
  return trapVerb("door", "stop", "stoped");
}

//--------------------------------vynyl-----------------------------------------------

bool statusVynyl() {
  return bossStat("vynyl");
}

//--------------------------------switches-----------------------------------------------

bool statusSwitches() {
  return bossStat("switches");
}

//-----------------------------------knock--------------------------------------------

void knockSensorController(){
//  while(1){
//    int val = analogRead(knock_analog);
//    if (val > KNOCK_TRESHOLD) {
//      Serial.println ( val );
//    }
//  }
  if ( analogRead(knock_analog) > KNOCK_TRESHOLD) {
    currentKnocks ++;
  }
  if(currentKnocks > 0){
    knockSampleLength--;
  }
  if(knockSampleLength == 0){
    knocks ++;
    currentKnocks = 0;
    knockSampleLength = KNOCK_SAMPLE_LENGTH;
  }
  if(knockDurationWindow == 0){
    if(knocks == KNOCKS_FOR_TRIGGER){
      actCigarettes();
    }
    currentKnocks = 0;
    knocks = 0;
    knockDurationWindow = KNOCK_DURATION_WINDOW;
  }
  if(currentKnocks || knocks){
    knockDurationWindow--;
  }
  delay(1);
}

bool actCigarettes() {
  knockSensorFlag = false;
  timer.deleteTimer( wallSoundTimerId );
  wallSoundTimerId = timer.setInterval(18000, wallVoice);
  return fireCigarettes();
}

void wallVoice() {
  attachMp3(0);
  if( knockSensorFlag ){
    mp3_play(1);
  } else {
    mp3_play(2);
  }
}
//-----------------------communication rutines-----------------------------------------

bool trapVerb(String trapName, String verb, String answer){
  while (Serial1.available()) {
      Serial1.read();
  }
  delay(50);
  digitalWrite(trapsRxTx, HIGH);
  delay(50);
  Serial1.print("{\"" + trapName + "\":\"" + verb + "\"}\n");
  delay(100);
  digitalWrite(trapsRxTx, LOW);
  delay(50);
  int counter = 0;
  int charCounter = 0;
  while(counter < 6000){
    while (Serial1.available()) {
      char inChar = (char)Serial1.read();
      if (charCounter == 0 && inChar != '{' ){
        continue;
      } else if (inChar == '\n') {
        trapStringComplete = true;
      } else {
        trapInputString += inChar;
        
      }
      charCounter++;
    }
    if(trapStringComplete){      
      char* stat;
      DynamicJsonBuffer  jsonBuffer;
      JsonObject & root = jsonBuffer.parseObject(trapInputString);
      trapInputString = "";
      if (root.success()) {
        stat = root[trapName];
        if( (String) stat == answer){
          return true;
        }
      }
      return false;
    }
    delay(1);
    counter ++;
  }
  trapInputString = "";
  return false;  
}

bool bossStat(String trapName){
  while (Serial1.available()) {
      Serial1.read();
  }
  delay(50);
  digitalWrite(trapsRxTx, HIGH);
  delay(50);
  Serial1.print("{\"" + trapName + "\":\"status\"}\n");
  delay(50);
  digitalWrite(trapsRxTx, LOW);
  delay(50);
  int counter = 0;
  while(counter < 3000){
    while (Serial1.available()) {
      char inChar = (char)Serial1.read();
      if (inChar == '\n') {
        trapStringComplete = true;
      } else {
        trapInputString += inChar;
      }
    }
    if(trapStringComplete){
      char* stat;
      DynamicJsonBuffer  jsonBuffer;
      JsonObject & root = jsonBuffer.parseObject(trapInputString);
      if (root.success()) {
        stat = root[trapName];
        if(stat){
          return true;
        }
      }
      return false;
    }
    delay(1);
    counter ++;
  }
  return false;  
}


//   {"lock":"stat"}
//   {"lock":"fire"}
//   {"lock":"open"}
//   {"lock":"arm"}
//   {"lock":"batt"}
//   {"lock":"gun"}

int lockVerb(String verb, String answer){
  delay(10);
  rfInputString = "";
  rfStringComplete = false;
  Serial2.println("{\"lock\":\"" + verb + "\"}");
  int counter = 0;
  while(counter < 7000){
    while (Serial2.available()) {
      char inChar = (char)Serial2.read();
      if (inChar == '\n') {
        rfStringComplete = true;
      } else {
        rfInputString += inChar;
      }
    }
    if(rfStringComplete){
      char* stat;
      DynamicJsonBuffer  jsonBuffer;
      JsonObject & root = jsonBuffer.parseObject(rfInputString);
      //rfInputString = "";
      rfStringComplete = false;
      if (root.success()) {
        stat = root["lock"];
        if(verb == "fire" || verb == "open" || verb == "arm"){
          if( (String) stat == answer){
            return 1;
          } else {
            return 0;
          }
        } else if(verb == "batt" || verb == "gun"){
          return String (stat).toInt();  
        } else if(verb == "stat"){
          return 1;  
        }
      }
      return 0;
    }
    delay(1);
    counter ++;
  }
  return 0;  
}


bool ledOff (int ledNumber){
  while (Serial1.available()) {
      Serial1.read();
  }
  delay(50);
  digitalWrite(trapsRxTx, HIGH);
  delay(50);
  Serial1.print("{\"led_off\":" + String(ledNumber) + "}\n");
  delay(100);
  digitalWrite(trapsRxTx, LOW);
  delay(50);
  int counter = 0;
  int charCounter = 0;
  while(counter < 6000){
    while (Serial1.available()) {
      char inChar = (char)Serial1.read();
      if (charCounter == 0 && inChar != '{' ){
        continue;
      } else if (inChar == '\n') {
        trapStringComplete = true;
      } else {
        trapInputString += inChar;
        
      }
      charCounter++;
    }
    if(trapStringComplete){      
      char* stat;
      DynamicJsonBuffer  jsonBuffer;
      JsonObject & root = jsonBuffer.parseObject(trapInputString);
      trapInputString = "";
      if (root.success()) {
        stat = root["led_off"];
        if( (String) stat == "off"){
          return true;
        }
      }
      return false;
    }
    delay(1);
    counter ++;
  }
  trapInputString = "";
  return false;  
}

bool firePost() {
  return trapVerb("post", "fire", "triggered");
}

bool statusPost() {
  return trapVerb("post", "status", "opened");
}

bool openPost() {
  return trapVerb("post", "open", "opened");
}

bool closePost() {
  return trapVerb("post", "close", "closed");
}


