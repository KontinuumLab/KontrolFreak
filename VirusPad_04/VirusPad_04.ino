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
int sensorThreshold[17];

int sensorMin[16] = {400, 500, 550, 480, 510, 540, 550, 400, 300, 450, 450, 500, 400, 450, 550, 400};

int sensorMax[16] = {700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700, 700};

int notes[16] = {10, 20, 30, 40, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71};
int MIDIchannel[16] = {16, 16, 16, 16, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};

// Currently pressed keys:
int activeSensors[16];

// Button pins:
int modeBtn = 14;

int diff[17];
int velocity;
int note;

int ccVolume;
int lastccVolume;

int playingMode;

int pot1;
int pot2;
int lastPot1;
int lastPot2;

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
  // Make variables for for loops:
  playingMode = digitalRead(modeBtn);
  int i;
  int j;
  for(i = 0; i < 16; i++){
    // Remember the last sensor reading at this position:
    lastSensorReadings[i] = sensorReadings[i];
    sensorReadings[i] = readMuxSingle(i);
    sensorReadings[i] = expFilter(sensorReadings[i], lastSensorReadings[i], 1024, 0.01);

    
    Serial.print(sensorReadings[i]);
    Serial.print(" - ");
  }
  for(i = 0; i < 4; i++){
    if(sensorReadings[i] > sensorMin[i] && lastSensorReadings[i] <= sensorMin[i]){
      usbMIDI.sendNoteOn(notes[i], 127, MIDIchannel[i]);
    }
  }
  if(playingMode == 0){ //Percussion mode:
    for(i = 4; i < 16; i++){
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
  else{ //Note mode:
    for(i = 4; i < 16; i++){
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
          // Move all values in the array one position to the right:
          for(j = 15; j > 4; j--){
            activeSensors[j] = activeSensors[j - 1]; 
          }
          // Save current sensor number at the leftmost position ([0]):
          activeSensors[4] = i;    
          // Send note on MIDI message for the note at the current position:
          usbMIDI.sendNoteOn(notes[i], 127, MIDIchannel[i]);  
        }
      }
      else if(sensorReadings[i] <= sensorMin[i] && lastSensorReadings[i] > sensorMin[i]){
      // Send note off MIDI message for the note at the current position:
        usbMIDI.sendNoteOn(notes[i], 0, MIDIchannel[i]);
        // Move all values right of the current position in the array, one position to the left:
        for(j = 4; j < 15; j++){
          if(activeSensors[j] == i){
            for(int k = j; k < 15; k++){
              activeSensors[k] = activeSensors[k + 1]; 
            }
            // Set the value at the rightmost position ([15]) to 0:
            activeSensors[15] = 0;
          }
        }
      }
      lastccVolume = ccVolume;
      ccVolume = map(sensorReadings[activeSensors[4]], sensorMin[activeSensors[4]], sensorMax[activeSensors[4]], 0, 127);
      if(ccVolume < 0){
        ccVolume = 0;
      }
      else if(ccVolume > 127){
        ccVolume = 127;
      }
      if(ccVolume != lastccVolume){
        usbMIDI.sendControlChange(2, ccVolume, MIDIchannel[activeSensors[4]]);
      }
    }
  }
  lastPot1 = pot1;
  lastPot2 = pot2;
  pot1 = map(analogRead(potmeter1Pin), 0, 1023, 0, 127);
  pot2 = map(analogRead(potmeter2Pin), 0, 1023, 0, 127);
  if(pot1 != lastPot1){
    usbMIDI.sendControlChange(111, pot1, 1);
  }
    if(pot2 != lastPot2){
    usbMIDI.sendControlChange(112, pot2, 1);
  }
  Serial.println();

}
// End main loop//

