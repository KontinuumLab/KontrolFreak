
int breathPin = 25;
int breathMin = 530;
int breathMax = 380;

int leftKeys[5] = {1, 3, 4, 16, 17};
int leftKeysThreshold[5] = {120, 120, 120, 135, 145};
int leftKeyVal[5];
int leftBtn = 8;
int leftBtnVal;

int leftAx1 = 14;
int leftAx2 = 24;

int lAx1Min = 1020;
int lAx1Cent = 533;
int lAx1Max = 5;
int lAx2Min = 8;
int lAx2Cent = 533;
int lAx2Max = 1018;

int lAx1Val;
int lAx2Val;

int rightKeys[5] = {18, 19, 22, 23, 0};
int rightKeysThreshold[5] = {115, 125, 120, 135, 145};
int rightBtnVal;
int rightBtn = 7;
int rightKeyVal[5];


int rightAx1 = 20;
int rightAx2 = 21;

int rAx1Min = 5;
int rAx1Cent = 506;
int rAx1Max = 1020;
int rAx2Min = 5;
int rAx2Cent = 533;
int rAx2Max = 1020;

int rAx1Val;
int rAx2Val;

int sensorBuffer = 5;

void setup() {
  pinMode(leftBtn, INPUT_PULLUP);
  pinMode(rightBtn, INPUT_PULLUP);
  Serial.begin(9600);

}

void loop() {
  int i;
  Serial.print("Breath: ");
  int breath = analogRead(breathPin);
  breath = map(breath, breathMin, breathMax, 0, 127);
  if(breath < 0){
    breath = 0;
  }
  else if(breath > 127){
    breath = 127;
  }
  Serial.println(breath);


  // LEFT CONTROLS:
  
  Serial.print("LEFT KEYS: ");
  for(i = 0; i < 5; i++){
    int key = touchRead(leftKeys[i]);
    if(key < leftKeysThreshold[i]){
      leftKeyVal[i] = 0;
    }
    else{
      leftKeyVal[i] = 1;
    }
    Serial.print(leftKeyVal[i]);
    Serial.print(" - ");
  }
  Serial.println();

  leftBtnVal = !digitalRead(leftBtn);
  Serial.print("Btn: ");
  Serial.print(leftBtnVal);
  Serial.print(" - ");

  lAx1Val = analogRead(leftAx1);
  if(lAx1Val > lAx1Cent){
    lAx1Val = map(lAx1Val, lAx1Min, lAx1Cent, 0, 63);
  }
  else if(lAx1Val < lAx1Cent){
    lAx1Val = map(lAx1Val, lAx1Cent, lAx1Max, 65, 127);
  }
  else{
    lAx1Val = 64;
  }
  if(lAx1Val < 0){
    lAx1Val = 0;
  }
  else if(lAx1Val > 127){
    lAx1Val = 127;
  }
  Serial.print(" pot1 - Ax1: ");
  Serial.print(lAx1Val);

 
  lAx2Val = analogRead(leftAx2);
  if(lAx2Val < lAx2Cent){
    lAx2Val = map(lAx2Val, lAx2Cent, lAx2Min, 0, 127);
    lAx2Val = constrain(lAx2Val, 0, 127);
    Serial.print(" pot1 - Ax2: IN ");
    Serial.print(lAx2Val);
  }
  else if(lAx2Val > lAx2Cent){
    lAx2Val = map(lAx2Val, lAx2Cent, lAx2Max, 0, 127);
    lAx2Val = constrain(lAx2Val, 0, 127);
    Serial.print(" pot1 - Ax2: OUT ");
    Serial.print(lAx2Val);
  }
  else{
    lAx2Val = 0;
    Serial.print(" pot1 - Ax2: XX ");
    Serial.print(lAx2Val);
  }
  Serial.println();


  // RIGHT CONTROLS:
  
  Serial.print("RIGHT KEYS: ");
  for(i = 0; i < 5; i++){
    int key = touchRead(rightKeys[i]);
    if(key < rightKeysThreshold[i]){
      rightKeyVal[i] = 0;
    }
    else{
      rightKeyVal[i] = 1;
    }
    Serial.print(rightKeyVal[i]);
    Serial.print(" - ");
  }
  Serial.println();

  rightBtnVal = !digitalRead(rightBtn);
  Serial.print("Btn: ");
  Serial.print(rightBtnVal);
  Serial.print(" - ");

  rAx1Val = analogRead(rightAx1);
  if(rAx1Val < rAx1Cent){
    rAx1Val = map(rAx1Val, rAx1Min, rAx1Cent, 0, 63);
  }
  else if(rAx1Val > rAx1Cent){
    rAx1Val = map(rAx1Val, rAx1Cent, rAx1Max, 65, 127);
  }
  else{
    rAx1Val = 64;
  }
  if(rAx1Val < 0){
    rAx1Val = 0;
  }
  else if(rAx1Val > 127){
    rAx1Val = 127;
  }
  Serial.print(" pot2 - Ax1: ");
  Serial.print(rAx1Val);

  rAx2Val = analogRead(rightAx2);
  if(rAx2Val < rAx2Cent){
    rAx2Val = map(rAx2Val, rAx2Cent, rAx2Min, 0, 127);
    rAx2Val = constrain(rAx2Val, 0, 127);
    Serial.print(" pot2 - Ax2: IN ");
    Serial.print(rAx2Val);
  }
  else if(rAx2Val > rAx2Cent){
    rAx2Val = map(rAx2Val, rAx2Cent, rAx2Max, 0, 127);
    rAx2Val = constrain(rAx2Val, 0, 127);
    Serial.print(" pot2 - Ax2: OUT ");
    Serial.print(rAx2Val);
  }
  else{
    rAx2Val = 0;
    Serial.print(" pot2 - Ax2: XX ");
    Serial.print(rAx2Val);
  }
  Serial.println();
  Serial.println();

//
  delay(100);
}
