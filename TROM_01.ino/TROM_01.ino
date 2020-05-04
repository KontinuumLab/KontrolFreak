
int slidePin = 23;
int breathPin = 24;
int lipPin = 16;


void setup() {
  Serial.begin(9600);

}

void loop() {

  Serial.print(analogRead(slidePin));
  Serial.print(" - ");
  Serial.print(analogRead(breathPin));
  Serial.print(" - ");
  Serial.println(touchRead(lipPin));
  delay(20);
  
} 
