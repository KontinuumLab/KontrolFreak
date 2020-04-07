// Multiplexer control pins:
int muxPin1 = 12;
int muxPin2 = 11;
int muxPin3 = 10;
int muxPin4 = 9;
int sensorPin = 15;

int potmeter1Pin = 20;
int potmeter2Pin = 21;

// current and last readings on keySensors:
int sensorReadings[17];
int lastSensorReadings[17];
int sensorThreshold[17] = {};

int sensorMin[16] = {400, 580, 610, 580, 490, 600, 640, 490, 440, 600, 500, 520, 400, 590, 630, 450};


// Currently pressed keys:
int activeSensors[16];

// Button pins:
int calibrateBtn = 14;

int diff[17];

// error variable for the exponential filter:
float error;


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

// Helper function used internally in the exponential filter:
float snapCurve(float x){
  float y = 1.0 / (x + 1.0);
  y = (1.0 - y) * 2.0;
  if(y > 1.0) {
    return 1.0;
  }
  return y;
}

// Exponential filter function. Input "snapMult" = speed setting. Lower (0.001) is slower. Higher (0.1) is faster:
int expFilter(int newValue, int lastValue, int resolution, float snapMult){
  unsigned int diff = abs(newValue - lastValue);
  error += ((newValue - lastValue) - error) * 0.4;
  float snap = snapCurve(diff * snapMult);
  float outputValue = lastValue;
  outputValue  += (newValue - lastValue) * snap;
  if(outputValue < 0.0){
    outputValue = 0.0;
  }
  else if(outputValue > resolution - 1){
    outputValue = resolution - 1;
  }
  return (int)outputValue;
}

// Main loop:
void loop(){
  // Make variables for for loops:
  int i;

  for(i = 0; i < 16; i++){
    // Remember the last sensor reading at this position:
    lastSensorReadings[i] = sensorReadings[i];
    sensorReadings[i] = readMuxSingle(i);
    sensorReadings[i] = expFilter(sensorReadings[i], lastSensorReadings[i], 1024, 0.01);

    
    Serial.print(sensorReadings[i]);
    Serial.print(" - ");
    if(sensorReadings[i] > lastSensorReadings[i]){
      diff[i] = sensorReadings[i] - lastSensorReadings[i];
      if(diff[i] > 3 && sensorReadings[i] > sensorMin[i]){
        int note = 75 - i;
        Serial.print("    note: ");
        Serial.print(note);
        delay(1000);
//        usbMIDI.sendNoteOn(note, 127, 1);
      }
    }
  }
  Serial.println();
//  Serial.print(analogRead(potmeter1Pin));
//  Serial.print(" - ");
//  Serial.println(analogRead(potmeter2Pin));
}

