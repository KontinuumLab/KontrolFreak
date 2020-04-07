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

int sensorMin[16] = {450, 590, 620, 580, 600, 640, 630, 490, 420, 620, 500, 520, 400, 590, 630, 450};

int sensorMax[16] = {550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550, 550};

// Currently pressed keys:
int activeSensors[16];

// Button pins:
int modeBtn = 14;

int diff[17];
int velocity;
int note;

int playingMode;

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
  pinMode(modeBtn, INPUT_PULLUP);
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

  playingMode = digitalRead(modeBtn);
  int i;

  for(i = 0; i < 16; i++){
    // Remember the last sensor reading at this position:
    lastSensorReadings[i] = sensorReadings[i];
    sensorReadings[i] = readMuxSingle(i);
    sensorReadings[i] = expFilter(sensorReadings[i], lastSensorReadings[i], 1024, 0.01);

    
    Serial.print(sensorReadings[i]);
    Serial.print(" - ");
  }
  if(playingMode == 0){
    for(i = 0; i < 16; i++){
      if(sensorReadings[i] > lastSensorReadings[i]){
        diff[i] = sensorReadings[i] - lastSensorReadings[i];
        if(diff[i] > 2 && sensorReadings[i] > sensorMin[i]){
          note = 35 + i;
          velocity = map(diff[i], 2, 15, 0, 127);
          if(velocity < 0){
            velocity = 0;
          }
          else if(velocity > 127){
            velocity = 127;
          }
          usbMIDI.sendNoteOn(note, velocity, 1);
        }
      }
    }
  }
  else{
    for(i = 0; i < 16; i++){
      if(sensorReadings[i] > lastSensorReadings[i]){
        diff[i] = sensorReadings[i] - lastSensorReadings[i];
        if(diff[i] > 2 && sensorReadings[i] > sensorMin[i]){
          note = 60 + i;
          velocity = map(diff[i], 2, 15, 0, 127);
          if(velocity < 0){
            velocity = 0;
          }
          else if(velocity > 127){
            velocity = 127;
          }
        }
        if(sensorReadings[i] > sensorMin[i] && lastSensorReadings[i] <= sensorMin[i]){
          usbMIDI.sendNoteOn(i + 60, velocity, 2);
        }
      }
      else if(sensorReadings[i] <= sensorMin[i] && lastSensorReadings[i] > sensorMin[i]){
        usbMIDI.sendNoteOn(i + 60, 0, 2);
      }
    }
  }
  Serial.println();

}
// End main loop//

