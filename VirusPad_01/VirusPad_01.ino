// Multiplexer control pins:
int muxPin1 = 12;
int muxPin2 = 11;
int muxPin3 = 10;
int muxPin4 = 9;
int sensorPin = 15;

int potmeter1Pin = 20;
int potmeter2Pin = 21;

void setup() {
  // Start the serial:
  Serial.begin(9600);
  // Set multiplexer control pins pinMode:
  pinMode(muxPin1,OUTPUT);
  pinMode(muxPin2,OUTPUT);
  pinMode(muxPin3,OUTPUT);
  pinMode(muxPin4,OUTPUT);
  
  digitalWrite(13, LOW);
}

// Function for accessing individual multiplexer sensors. Input sensor number: 
int readMuxSingle(int number){
  digitalWrite(muxPin1, bitRead(number, 0)); 
  digitalWrite(muxPin2, bitRead(number, 1));
  digitalWrite(muxPin3, bitRead(number, 2));
  digitalWrite(muxPin4, bitRead(number, 3));
  int value = analogRead(sensorPin);
  return value;
}

// Main loop:
void loop(){
  // Make variables for for loops:
  int i;

  for(i = 0; i < 16; i++){
    Serial.print(readMuxSingle(i));
    Serial.print(" - ");
  }
  Serial.println();
  Serial.print(analogRead(potmeter1Pin));
  Serial.print(" - ");
  Serial.println(analogRead(potmeter2Pin));
  delay(5);
}
// End main loop//



 
