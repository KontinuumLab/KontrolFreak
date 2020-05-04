
int slidePin = 23;  // reads 700 - 200
int breathPin = 24; // reads 250 - 50
int lipPin = 16;    // reads 170 - 270

int slideMin = 160;
int slideMax = 700;
int slideReading;
int lastSlideReading;
int slideOut;
int lastSlideOut;
int pitchBend;

float slideFactor = 3.0;

int breathMin = 225;
int breathMax = 65;
int breathReading;
int lastBreathReading;
int breathOut;
int lastBreathOut;

int lipMin = 3000;
int lipMax = 2000;
int lipReading;
int lastLipReading;
int lipOut;
int lastLipOut;

int baseNote = 30;
int transpose;
int harmonics[5] = {0, 7, 12, 16, 19};
int currentNote;
int lastNote;

float error;

void setup() {

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


void loop() {
  lastBreathReading = breathReading;
  lastBreathOut = breathOut;
  breathReading = analogRead(breathPin);
  breathReading = expFilter(breathReading, lastBreathReading, 1024, 0.005);
  breathOut = map(breathReading, breathMin, breathMax, 0, 127);
  breathOut = constrain(breathOut, 0, 127);


  lastSlideReading = slideReading;
  lastSlideOut = slideOut;
  slideReading = analogRead(slidePin);
  slideReading = expFilter(slideReading, lastSlideReading, 1024, 0.005);
  
  float filteredFloat = fscale(float(slideMin), float(slideMax), 0.0, 127.0, float(slideReading), slideFactor);
  slideReading = int(filteredFloat); // * 100;
  
//  slideOut = map(slideReading, slideMin, slideMax, 0, 127);
  slideOut = constrain(slideReading, 0, 127);
  pitchBend = slideOut - 64;

  
  lastLipReading = lipReading;
  lastLipOut = lipOut;
  lipReading = touchRead(lipPin);
  lipReading = expFilter(lipReading, lastLipReading, 3000, 0.005);
  lipOut = map(lipReading, lipMin, lipMax, 0, 4);
  lipOut = constrain(lipOut, 0, 4);
  transpose = harmonics[lipOut];

  lastNote = currentNote;
  currentNote = baseNote + transpose; // + valve calculation  


  if(breathOut != 0){
    if(slideOut != lastSlideOut){
      usbMIDI.sendPitchBend(pitchBend<<7, 1);
    }
    if(lastBreathOut != breathOut){
      usbMIDI.sendControlChange(2, breathOut, 1);
    }
    
    if(lastBreathOut == 0){
      usbMIDI.sendPitchBend(pitchBend<<7, 1);
      usbMIDI.sendNoteOn(currentNote, 127, 1);
    }
    if(lastNote != currentNote){
      usbMIDI.sendNoteOn(lastNote, 0, 1);
      usbMIDI.sendNoteOn(currentNote, 127, 1);
    }
  }
  else if(lastBreathOut != 0){
    usbMIDI.sendNoteOn(currentNote, 0, 1);
    usbMIDI.sendNoteOn(lastNote, 0, 1);
  }
  
} 



float fscale( float originalMin, float originalMax, float newBegin, float
newEnd, float inputValue, float curve){

  float OriginalRange = 0;
  float NewRange = 0;
  float zeroRefCurVal = 0;
  float normalizedCurVal = 0;
  float rangedValue = 0;
  boolean invFlag = 0;


  // condition curve parameter
  // limit range

  if (curve > 10) curve = 10;
  if (curve < -10) curve = -10;

  curve = (curve * -.1) ; // - invert and scale - this seems more intuitive - postive numbers give more weight to high end on output
  curve = pow(10, curve); // convert linear scale into lograthimic exponent for other pow function

 
  // Check for out of range inputValues
  if (inputValue < originalMin) {
    inputValue = originalMin;
  }
  if (inputValue > originalMax) {
    inputValue = originalMax;
  }

  // Zero Refference the values
  OriginalRange = originalMax - originalMin;

  if (newEnd > newBegin){
    NewRange = newEnd - newBegin;
  }
  else
  {
    NewRange = newBegin - newEnd;
    invFlag = 1;
  }

  zeroRefCurVal = inputValue - originalMin;
  normalizedCurVal  =  zeroRefCurVal / OriginalRange;   // normalize to 0 - 1 float


  // Check for originalMin > originalMax  - the math for all other cases i.e. negative numbers seems to work out fine
  if (originalMin > originalMax ) {
    return 0;
  }

  if (invFlag == 0){
    rangedValue =  (pow(normalizedCurVal, curve) * NewRange) + newBegin;

  }
  else     // invert the ranges
  { 
    rangedValue =  newBegin - (pow(normalizedCurVal, curve) * NewRange);
  }

  return rangedValue;
}

