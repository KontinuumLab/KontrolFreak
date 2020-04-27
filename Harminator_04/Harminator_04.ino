
int breathPin = 25;
int breathMin = 530;
int breathMax = 380;
int breath;
int lastBreath;

int lipPin = 15;
int lipMin = 150;
int lipMax = 270;
int lipReading;
int lastLipReading;
int lipOut;
int lastLipOut;

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
int lastLAx2Val;
int leftPitchBend;
int lastLeftPitchBend;

int leftOctave;

int rightKeys[5] = {18, 19, 22, 23, 0};
int rightKeysThreshold[5] = {130, 125, 125, 135, 145};
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
int lastRAx2Val;
int rightPitchBend;
int lastRightPitchBend;

int rightOctave;

int currentNote;

int sensorBuffer = 5;

int notes[5] = {50, 53, 55, 57, 60};

int stopped = 1;

void setup() {
  pinMode(leftBtn, INPUT_PULLUP);
  pinMode(rightBtn, INPUT_PULLUP);
//  Serial.begin(9600);

}


// Helper function for the exponential filter function:
float snapCurve(float x){
  float y = 1.0 / (x + 1.0);
  y = (1.0 - y) * 2.0;
  if(y > 1.0) {
    return 1.0;
  }
  return y;
}

// Main exponential filter function. Input "snapMult" = speed setting. 0.001 = slow / 0.1 = fast:
int expFilter(int newValue, int lastValue, int resolution, float snapMult){
  unsigned int diff = abs(newValue - lastValue);
//  error += ((newValue - lastValue) - error) * 0.4;
  float snap = snapCurve(diff * snapMult);
  float outputValue = lastValue;
  outputValue  += (newValue - lastValue) * snap;
  if(outputValue < 0.0){
    outputValue = 0.0;
  }
//  else if(outputValue > resolution - 1){
//    outputValue = resolution - 1;
//  }
  return (int)outputValue;
}



void loop() {
  int i;
//  Serial.print("Breath: ");
  lastBreath = breath;
  breath = analogRead(breathPin);
  breath = expFilter(breath, lastBreath, 1024, 0.005);
  breath = map(breath, breathMin, breathMax, 0, 127);
  breath = constrain(breath, 0, 127);
  if(breath != lastBreath){
    usbMIDI.sendControlChange(2, breath, 2);
    usbMIDI.sendControlChange(2, breath, 3);
  }


  lastLipReading = lipReading;
  lipReading = touchRead(lipPin);
  lipReading = expFilter(lipReading, lastLipReading, 1024, 0.005);
  lipReading = map(lipReading, lipMin, lipMax, 60, 127);
  lipReading = constrain(lipReading, 60, 127);
  lastLipOut = lipOut;
  lipOut = lipReading;
  if(lipReading != lastLipReading){
    usbMIDI.sendControlChange(110, lipOut, 2);
//    usbMIDI.sendControlChange(110, lipOut, 3);
  }
  
  // LEFT CONTROLS:
  lastLeftBtnVal = leftBtnVal;
  leftBtnVal = !digitalRead(leftBtn);
  if(leftBtnVal == 1 && lastLeftBtnVal == 0){
    delay(2);
    leftBtnVal = !digitalRead(leftBtn);
    if(leftBtnVal == 1){
      if(stopped == 0){
        usbMIDI.sendNoteOn(20, 127, 16);
      }
      else{
        usbMIDI.sendNoteOn(10, 127, 16);
        stopped = 0;
      }
    }
  }
    

  if(breath != 0){
    if(lastBreath == 0){
      for(i = 0; i < 5; i++){
        if(leftKeyVal[i] == 1){
          currentNote = notes[i];
          usbMIDI.sendNoteOn(currentNote, 127, 2);
        }
      }
    }
    for(i = 0; i < 5; i++){
      currentNote = notes[i];
      lastLeftKeyVal[i] = leftKeyVal[i];
      int key = touchRead(leftKeys[i]);
      if(key < leftKeysThreshold[i]){
        leftKeyVal[i] = 0;
      }
      else{
        leftKeyVal[i] = 1;
      }
      if(leftKeyVal[i] == 1 & lastLeftKeyVal[i] == 0){
        usbMIDI.sendNoteOn(currentNote, 127, 2);
      }
      else if(leftKeyVal[i] == 0 & lastLeftKeyVal[i] == 1){
        usbMIDI.sendNoteOn(currentNote, 0, 2);
      }
    }
  }
  else{
    if(lastBreath != 0){
      usbMIDI.sendControlChange(123, 0, 2);
    }
  }


  lastLAx1Read = lAx1Read;
  lAx1Read = analogRead(leftAx1);
  lAx1Read = expFilter(lAx1Read, lastLAx1Read, 1024, 0.005);
  lastLAx2Read = lAx2Read;
  lAx2Read = analogRead(leftAx2);
  lAx2Read = expFilter(lAx2Read, lastLAx2Read, 1024, 0.005);
  
  lastLeftPitchBend = leftPitchBend;
  
  if(lAx1Read != lastLAx1Read){
    if(lAx1Read > lAx1Cent){
      lAx1Val = map(lAx1Read, lAx1Min, lAx1Cent, 0, 63);
      lAx1Val = constrain(lAx1Val, 0, 63);
      lAx1Val = lAx1Val - 64;
    }
    else if(lAx1Read < lAx1Cent){
      lAx1Val = map(lAx1Read, lAx1Cent, lAx1Max, 65, 127);
      lAx1Val = constrain(lAx1Val, 65, 127);
      lAx1Val = lAx1Val - 64;
    }
    else{
      lAx1Val = 64;
      lAx1Val = lAx1Val - 64;     
    }
  }

  if(lAx2Read != lastLAx2Read){
    lastLAx2Val = lAx2Val;
    if(lAx2Read < lAx2Cent){
      lAx2Val = map(lAx2Read, lAx2Cent, lAx2Min, 33, 64);
      lAx2Val = constrain(lAx2Val, 33, 64);
      lAx2Val = lAx2Val - 32;
    }
    else if(lAx2Read > lAx2Cent){
      lAx2Val = map(lAx2Read, lAx2Cent, lAx2Max, 33, 0);
      lAx2Val = constrain(lAx2Val, 0, 33);
      lAx2Val = lAx2Val - 32;
    }
    else{
      lAx2Val = 0;
    }
  }

  if(abs(lAx1Val) >= abs(lAx2Val)){
    leftPitchBend = lAx1Val;
  }
  else{
    leftPitchBend = lAx2Val;
  }
  if(leftPitchBend != lastLeftPitchBend){
    usbMIDI.sendPitchBend(leftPitchBend<<7, 2);
  }


  // RIGHT CONTROLS:
  lastRightBtnVal = rightBtnVal;
  rightBtnVal = !digitalRead(rightBtn);
  if(rightBtnVal == 1 && lastRightBtnVal == 0){
    delay(2);
    rightBtnVal = !digitalRead(rightBtn);
    if(rightBtnVal == 1){
      if(stopped == 0){
        usbMIDI.sendNoteOn(30, 127, 16);
        stopped = 1;
      }
      else{
        usbMIDI.sendNoteOn(40, 127, 16);
        stopped = 0;
      }
    }
  }

  if(breath != 0){
    if(lastBreath == 0){
      for(i = 0; i < 5; i++){
        if(rightKeyVal[i] == 1){
          currentNote = notes[i];
          usbMIDI.sendNoteOn(currentNote, 127, 3);
        }
      }
    }
    for(i = 0; i < 5; i++){
      currentNote = notes[i];
      lastRightKeyVal[i] = rightKeyVal[i];
      int key = touchRead(rightKeys[i]);
      if(key < rightKeysThreshold[i]){
        rightKeyVal[i] = 0;
      }
      else{
        rightKeyVal[i] = 1;
      }
      if(rightKeyVal[i] == 1 & lastRightKeyVal[i] == 0){
        usbMIDI.sendNoteOn(currentNote, 127, 3);
      }
      else if(rightKeyVal[i] == 0 & lastRightKeyVal[i] == 1){
        usbMIDI.sendNoteOn(currentNote, 0, 3);
      }
    }
  }
  else{
    if(lastBreath != 0){
      usbMIDI.sendControlChange(123, 0, 3);
    }
  }

  lastRAx1Read = rAx1Read;
  rAx1Read = analogRead(rightAx1);
  rAx1Read = expFilter(rAx1Read, lastRAx1Read, 1024, 0.005);
  lastRAx2Read = rAx2Read;
  rAx2Read = analogRead(rightAx2);
  rAx2Read = expFilter(rAx2Read, lastRAx2Read, 1024, 0.005);
  
  lastRightPitchBend = rightPitchBend;
  
  if(rAx1Read != lastRAx1Read){
    if(rAx1Read < rAx1Cent){
      rAx1Val = map(rAx1Read, rAx1Min, rAx1Cent, 0, 63);
      rAx1Val = constrain(rAx1Val, 0, 63);
      rAx1Val = rAx1Val - 64;
    }
    else if(rAx1Read > rAx1Cent){
      rAx1Val = map(rAx1Read, rAx1Cent, rAx1Max, 65, 127);
      rAx1Val = constrain(rAx1Val, 65, 127);
      rAx1Val = rAx1Val - 64;
    }
    else{
      rAx1Val = 64;
      rAx1Val = rAx1Val - 64;
    }
  }

  if(rAx2Read != lastRAx2Read){
    lastRAx2Val = rAx2Val;
    if(rAx2Read < rAx2Cent){
      rAx2Val = map(rAx2Read, rAx2Cent, rAx2Min, 33, 64);
      rAx2Val = constrain(rAx2Val, 33, 64);
      rAx2Val = rAx2Val - 32;
    }
    else if(rAx2Read > rAx2Cent){
      rAx2Val = map(rAx2Read, rAx2Cent, rAx2Max, 33, 0);
      rAx2Val = constrain(rAx2Val, 0, 33);
      rAx2Val = rAx2Val - 32;
    }
    else{
      rAx2Val = 0;
    }
  }

  if(abs(rAx1Val) >= abs(rAx2Val)){
    rightPitchBend = rAx1Val;
  }
  else{
    rightPitchBend = rAx2Val;
  }
  if(rightPitchBend != lastRightPitchBend){
    usbMIDI.sendPitchBend(rightPitchBend<<7, 3);
  }



//
//  delay(2);
}
