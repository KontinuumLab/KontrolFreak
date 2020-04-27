
int breathPin = 25;
int breathMin = 530;
int breathMax = 380;
int breath;
int lastBreath;

int leftKeys[5] = {1, 3, 4, 16, 17};
int leftKeysThreshold[5] = {120, 120, 120, 135, 145};
int leftKeyVal[5];
int lastLeftKeyVal[5];

int leftBtn = 8;
int leftBtnVal;
int lastLeftBtnVal;

int leftAx1 = 14;
int leftAx2 = 24;

int lAx1Min = 1020;
int lAx1Cent = 533;
int lAx1Max = 5;
int lAx2Min = 8;
int lAx2Cent = 533;
int lAx2Max = 1018;

int lAx1Read;
int lAx1Val;
int lAx2Read;
int lAx2Val;
int lastLAx1Read;
int lastLAx2Read;

int leftOctave;

int rightKeys[5] = {18, 19, 22, 23, 0};
int rightKeysThreshold[5] = {120, 125, 120, 135, 145};
int rightKeyVal[5];
int lastRightKeyVal[5];

int rightBtn = 7;
int rightBtnVal;
int lastRightBtnVal;

int rightAx1 = 20;
int rightAx2 = 21;

int rAx1Min = 5;
int rAx1Cent = 506;
int rAx1Max = 1020;
int rAx2Min = 5;
int rAx2Cent = 533;
int rAx2Max = 1020;

int rAx1Read;
int rAx1Val;
int rAx2Read;
int rAx2Val;
int lastRAx1Read;
int lastRAx2Read;

int rightOctave;

int currentNote;

int sensorBuffer = 5;

int notes[5] = {50, 53, 55, 57, 60};

void setup() {
  pinMode(leftBtn, INPUT_PULLUP);
  pinMode(rightBtn, INPUT_PULLUP);
//  Serial.begin(9600);

}

void loop() {
  int i;
//  Serial.print("Breath: ");
  lastBreath = breath;
  breath = analogRead(breathPin);
  breath = map(breath, breathMin, breathMax, 0, 127);
  if(breath < 0){
    breath = 0;
  }
  else if(breath > 127){
    breath = 127;
  }
  if(breath != lastBreath){
    usbMIDI.sendControlChange(2, breath, 3);
    usbMIDI.sendControlChange(2, breath, 2);
  }
//  Serial.println(breath);


  // LEFT CONTROLS:
  
//  Serial.print("LEFT KEYS: ");
  if(breath != 0){
    if(lastBreath == 0){
      for(i = 0; i < 5; i++){
        if(leftKeyVal[i] == 1){
          currentNote = notes[i] + leftOctave;
          usbMIDI.sendNoteOn(currentNote, 127, 3);
        }
      }
    }
    for(i = 0; i < 5; i++){
      currentNote = notes[i] + leftOctave;
      lastLeftKeyVal[i] = leftKeyVal[i];
      int key = touchRead(leftKeys[i]);
      if(key < leftKeysThreshold[i]){
        leftKeyVal[i] = 0;
      }
      else{
        leftKeyVal[i] = 1;
      }
      if(leftKeyVal[i] == 1 & lastLeftKeyVal[i] == 0){
        usbMIDI.sendNoteOn(currentNote, 127, 3);
      }
      else if(leftKeyVal[i] == 0 & lastLeftKeyVal[i] == 1){
        usbMIDI.sendNoteOn(currentNote, 0, 3);
      }
//      Serial.print(leftKeyVal[i]);
//      Serial.print(" - ");
    }
  }
  else{
    if(lastBreath != 0){
      usbMIDI.sendControlChange(123, 0, 3);
    }
  }
//  Serial.println();
  lastLeftBtnVal = leftBtnVal;
  leftBtnVal = !digitalRead(leftBtn);
  if(leftBtnVal != lastLeftBtnVal){
    leftOctave = 12 * leftBtnVal; 
  }
  
//  Serial.print("Btn: ");
//  Serial.print(leftBtnVal);
//  Serial.print(" - ");
  lastLAx1Read = lAx1Read;
  lAx1Read = analogRead(leftAx1);
  if(lAx1Read != lastLAx1Read){
    if(lAx1Read > lAx1Cent){
      lAx1Val = map(lAx1Read, lAx1Min, lAx1Cent, 0, 63);
      lAx1Val = constrain(lAx1Val, 0, 63);
      lAx1Val = lAx1Val - 64;
      usbMIDI.sendPitchBend(lAx1Val<<7, 3);
    }
    else if(lAx1Read < lAx1Cent){
      lAx1Val = map(lAx1Read, lAx1Cent, lAx1Max, 65, 127);
      lAx1Val = constrain(lAx1Val, 65, 127);
      lAx1Val = lAx1Val - 64;
      usbMIDI.sendPitchBend(lAx1Val<<7, 3);
    }
    else{
      lAx1Val = 64;
      lAx1Val = lAx1Val - 64;
      usbMIDI.sendPitchBend(lAx1Val<<7, 3);
    }
  }
//  Serial.print(" pot1 - Ax1: ");
//  Serial.print(lAx1Val);

  lastLAx2Read = lAx2Read;
  lAx2Read = analogRead(leftAx2);
  if(lAx2Read != lastLAx2Read){
    if(lAx2Read < lAx2Cent){
      lAx2Val = map(lAx2Read, lAx2Cent, lAx2Min, 0, 127);
      lAx2Val = constrain(lAx2Val, 0, 127);
      usbMIDI.sendControlChange(110, lAx2Val, 3);
//      Serial.print(" pot1 - Ax2: IN ");
//      Serial.print(lAx2Val);
    }
    else if(lAx2Read > lAx2Cent){
      lAx2Val = map(lAx2Read, lAx2Cent, lAx2Max, 0, 127);
      lAx2Val = constrain(lAx2Val, 0, 127);
      usbMIDI.sendControlChange(111, lAx2Val, 3);
//      Serial.print(" pot1 - Ax2: OUT ");
//      Serial.print(lAx2Val);
    }
    else{
      lAx2Val = 0;
      usbMIDI.sendControlChange(110, lAx2Val, 3);
      usbMIDI.sendControlChange(111, lAx2Val, 3);
//      Serial.print(" pot1 - Ax2: XX ");
//      Serial.print(lAx2Val);
    }
  }
//  Serial.println();



  // RIGHT CONTROLS:
  
//  Serial.print("LEFT KEYS: ");
  if(breath != 0){
    if(lastBreath == 0){
      for(i = 0; i < 5; i++){
        if(rightKeyVal[i] == 1){
          currentNote = notes[i] + rightOctave;
          usbMIDI.sendNoteOn(currentNote, 127, 2);
        }
      }
    }
    for(i = 0; i < 5; i++){
      currentNote = notes[i] + rightOctave;
      lastRightKeyVal[i] = rightKeyVal[i];
      int key = touchRead(rightKeys[i]);
      if(key < rightKeysThreshold[i]){
        rightKeyVal[i] = 0;
      }
      else{
        rightKeyVal[i] = 1;
      }
      if(rightKeyVal[i] == 1 & lastRightKeyVal[i] == 0){
        usbMIDI.sendNoteOn(currentNote, 127, 2);
      }
      else if(rightKeyVal[i] == 0 & lastRightKeyVal[i] == 1){
        usbMIDI.sendNoteOn(currentNote, 0, 2);
      }
//      Serial.print(leftKeyVal[i]);
//      Serial.print(" - ");
    }
  }
  else{
    if(lastBreath != 0){
      usbMIDI.sendControlChange(123, 0, 2);
    }
  }
//  Serial.println();
  lastRightBtnVal = rightBtnVal;
  rightBtnVal = !digitalRead(rightBtn);
  if(rightBtnVal != lastRightBtnVal){
    rightOctave = 12 * rightBtnVal; 
  }
  
//  Serial.print("Btn: ");
//  Serial.print(leftBtnVal);
//  Serial.print(" - ");
  lastRAx1Read = rAx1Read;
  rAx1Read = analogRead(rightAx1);
  if(rAx1Read != lastRAx1Read){
    if(rAx1Read < rAx1Cent){
      rAx1Val = map(rAx1Read, rAx1Min, rAx1Cent, 0, 63);
      rAx1Val = constrain(rAx1Val, 0, 63);
      rAx1Val = rAx1Val - 64;
      usbMIDI.sendPitchBend(rAx1Val<<7, 2);
    }
    else if(rAx1Read > rAx1Cent){
      rAx1Val = map(rAx1Read, rAx1Cent, rAx1Max, 65, 127);
      rAx1Val = constrain(rAx1Val, 65, 127);
      rAx1Val = rAx1Val - 64;
      usbMIDI.sendPitchBend(rAx1Val<<7, 2);
    }
    else{
      rAx1Val = 64;
      rAx1Val = rAx1Val - 64;
      usbMIDI.sendPitchBend(rAx1Val<<7, 2);
    }
  }
//  Serial.print(" pot1 - Ax1: ");
//  Serial.print(lAx1Val);

  lastRAx2Read = rAx2Read;
  rAx2Read = analogRead(rightAx2);
  if(rAx2Read != lastRAx2Read){
    if(rAx2Read < rAx2Cent){
      rAx2Val = map(rAx2Read, rAx2Cent, rAx2Min, 0, 127);
      rAx2Val = constrain(rAx2Val, 0, 127);
      usbMIDI.sendControlChange(110, rAx2Val, 2);
//      Serial.print(" pot1 - Ax2: IN ");
//      Serial.print(lAx2Val);
    }
    else if(rAx2Read > rAx2Cent){
      rAx2Val = map(rAx2Read, rAx2Cent, rAx2Max, 0, 127);
      rAx2Val = constrain(rAx2Val, 0, 127);
      usbMIDI.sendControlChange(111, rAx2Val, 2);
//      Serial.print(" pot1 - Ax2: OUT ");
//      Serial.print(lAx2Val);
    }
    else{
      rAx2Val = 0;
      usbMIDI.sendControlChange(110, rAx2Val, 2);
      usbMIDI.sendControlChange(111, rAx2Val, 2);
//      Serial.print(" pot1 - Ax2: XX ");
//      Serial.print(lAx2Val);
    }
  }
//  Serial.println();



//
//  delay(100);
}
