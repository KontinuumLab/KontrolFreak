
int breathPin = 25;

int leftKeys[5] = {1, 3, 4, 16, 17};
int leftBtn = 8;
int leftAx1 = 14;
int leftAx2 = 24;

int rightKeys[5] = {18, 19, 22, 23, 0};
int rightBtn = 7;
int rightAx1 = 20;
int rightAx2 = 21;



void setup() {
  pinMode(leftBtn, INPUT_PULLUP);
  pinMode(rightBtn, INPUT_PULLUP);
  Serial.begin(9600);

}

void loop() {
  int i;
  Serial.print("Breath: ");
  Serial.println(analogRead(breathPin));
  
  Serial.print("LEFT KEYS: ");
  for(i = 0; i < 5; i++){
    Serial.print(touchRead(leftKeys[i]));
    Serial.print(" - ");
  }
  Serial.println();
  Serial.print("Btn: ");
  Serial.print(digitalRead(leftBtn));
  Serial.print(" - ");

  Serial.print("pot1 - Ax1: ");
  Serial.print(analogRead(leftAx1));
  Serial.print(" - Ax2: ");
  Serial.print(analogRead(leftAx2));
  Serial.println();


  Serial.print("RIGHT KEYS: ");
  for(i = 0; i < 5; i++){
    Serial.print(touchRead(rightKeys[i]));
    Serial.print(" - ");
  }
  Serial.println();
  Serial.print("Btn: ");
  Serial.print(digitalRead(rightBtn));
  Serial.print(" - ");

  Serial.print("pot1 - Ax1: ");
  Serial.print(analogRead(rightAx1));
  Serial.print(" - Ax2: ");
  Serial.print(analogRead(rightAx2));
  Serial.println();
  Serial.println();
//
  delay(100);
}
