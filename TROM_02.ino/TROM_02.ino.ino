
int slidePin = 23;  // reads 700 - 200
int breathPin = 24; // reads 250 - 50
int lipPin = 16;    // reads 170 - 270

int slideMin = 700;
int slideMax = 200;
int slideReading;
int lastSlideReading;
int slideOut;
int lastSlideOut;

int breathMin = 225;
int breathMax = 65;
int breathReading;
int lastBreathReading;
int breathOut;
int lastBreathOut;

int lipMin = 1900;
int lipMax = 2600;
int lipReading;
int lastLipReading;
int lipOut;
int lastLipOut;

float error;

void setup() {
  Serial.begin(9600);

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
  breathReading = analogRead(breathPin);
  breathReading = expFilter(breathReading, lastBreathReading, 1024, 0.005);
  breathOut = map(breathReading, breathMin, breathMax, 0, 127);
  breathOut = constrain(breathOut, 0, 127);
  Serial.print(breathOut);
  Serial.print(" - ");

  lastSlideReading = slideReading;
  slideReading = analogRead(slidePin);
  slideReading = expFilter(slideReading, lastSlideReading, 1024, 0.005);
  slideOut = map(slideReading, slideMin, slideMax, 0, 6);
  slideOut = constrain(slideOut, 0, 6);
  Serial.print(slideOut);
  Serial.print(" - ");

  lastLipReading = lipReading;
  lipReading = touchRead(lipPin);
  lipReading = expFilter(lipReading, lastLipReading, 3000, 0.005);
  lipOut = map(lipReading, lipMin, lipMax, 0, 5);
  lipOut = constrain(lipOut, 0, 5);
  Serial.println(lipOut);
  
  delay(20);
  
} 
