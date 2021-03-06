                                                                                                                                                                                                                                                                                                                                             #define mic A1
#define vynyl A0
#define out 9


unsigned long sum = 0;
int sumCounter = 0;
int delayCounter = 0;

const int VYNYL_REF = 500;
const int VYNYL_CYCL = 10;
const int MIC_DEL = 8000;

void setup() {
  pinMode (mic, INPUT_PULLUP);
  pinMode (out, OUTPUT);
  digitalWrite (out, LOW);
  Serial.begin (9600);
}

void loop() {
  sum += analogRead(vynyl);
  sumCounter++;
  if (sumCounter > VYNYL_CYCL) {
    Serial.println (sum/VYNYL_CYCL);
    if( sum / VYNYL_CYCL > VYNYL_REF ) {
      while(delayCounter < MIC_DEL) {
        if ( !digitalRead (mic) ) {
          digitalWrite (out, HIGH);
        }      
        delayCounter ++;
        delay(1);  
      }
    } else {
      digitalWrite (out, LOW);
    }
    delayCounter = 0;
    sumCounter = 0;
    sum = 0;
  }
}
