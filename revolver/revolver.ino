#include <IRremote.h>

#define trigger 9
#define battery A1
#define irLed   3
#define led 13

int freq = 36;
IRsend irsend;

void setup() {
  pinMode(trigger, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if( !digitalRead(trigger) ){
    while(!digitalRead(trigger))
    digitalWrite(led, HIGH);
    unsigned int data[2];
    unsigned int volt =  map( analogRead( battery ), 750, 1023, 0, 100 );
    Serial.println (volt, DEC);

    irsend.sendSony(volt, 12);
    delay(1000);
    digitalWrite(led, LOW);
  }
}
