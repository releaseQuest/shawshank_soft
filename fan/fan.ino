#include "AccelStepper.h"

#define in0 2
#define in1 3

#define stp 5
#define dir 4

AccelStepper stepper(AccelStepper::DRIVER, stp, dir);

bool inputSyatusFlag = false;

void setup() {
  pinMode(stp, OUTPUT);
  pinMode(dir, OUTPUT);

  pinMode(in0, INPUT);
  pinMode(in1, INPUT);
  
  
 
  //while(1);
  
    
}

void loop() {

 if( digitalRead(in0) != digitalRead(in1) && !inputSyatusFlag) {
    delay(10);
    if( digitalRead(in0) != digitalRead(in1) ) {
      inputSyatusFlag = true;
      stepper.setMaxSpeed(300);
      stepper.setAcceleration(50);
      stepper.move(4000);
      stepper.run();
    }
  } else if( digitalRead(in0) == digitalRead(in1) && inputSyatusFlag) {
    delay(10);
    if( digitalRead(in0) == digitalRead(in1) ) {
      inputSyatusFlag = false;
     // stepper.disableOutputs();
      stepper.stop();
      stepper.setCurrentPosition(0);
    }
  }

  if( stepper.isRunning() && inputSyatusFlag){
    if (stepper.distanceToGo() <= 1000){
      stepper.move(3000);
    }
    stepper.run();
  } 
}
