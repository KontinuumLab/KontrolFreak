
int fsrPin = 16;
int fsrReading;
int lastFsrReading;

int fsrThreshold = 600;
int fsrMax = 620;
int hhPedal;
int hhValue = 44;
int hhVelocity;

int piezoPins[5] = {19, A11, A13, A10, 18};
int piezoReadings[5];

//int midiValues[5] = {46, 49, 37, 45, 35}; // Natural Drum Kit
//int midiValues[5] = {46, 49, 40, 43, 35}; // Subliminal Kit
int midiValues[5] = {42, 46, 38, 41, 36}; // Drums Kit

void setup() {
  Serial.begin(9600);

}

void loop() {
  delay(1);
  
  lastFsrReading = fsrReading;
  fsrReading = analogRead(fsrPin);
  if(fsrReading >= fsrThreshold ){
    hhPedal = 1;
    if(lastFsrReading < fsrThreshold){
      int tempVel = fsrReading - lastFsrReading;
      hhVelocity = map(tempVel, 0, 100, 0, 127);
      if(hhVelocity > 127){
        hhVelocity = 127;
      }
      Serial.print(lastFsrReading);
      Serial.print(" - ");
      Serial.print(fsrReading);
      Serial.print(" - ");
      Serial.println(hhVelocity);
      delay(1);
      
      usbMIDI.sendNoteOn(hhValue, hhVelocity, 1);
      return;
    }
  }
  else{
    hhPedal = 0;
  }
  
  int i;
  for(i = 0; i < 5; i++){
    piezoReadings[i] = analogRead(piezoPins[i]);
    Serial.print(piezoReadings[i]);
    Serial.print(" - ");
    if(piezoReadings[i] > 120){
      int sendValue = map(piezoReadings[i], 120, 800, 0, 127);
      if(sendValue > 127){
        sendValue = 127;
      }
      if(i == 0){
        if(hhPedal == 0){
          usbMIDI.sendNoteOn(midiValues[i], sendValue, 1);
        }
        else{
          usbMIDI.sendNoteOn(44, sendValue, 1);
          usbMIDI.sendNoteOn(42, 0, 1);
        }
      }
      else{
        usbMIDI.sendNoteOn(midiValues[i], sendValue, 1);
      }
      Serial.println(); 
      delay(1);
      return;
    }
  }
  Serial.println();

}
