#define breakBeam 3
#define LED 13
#define solenoid 2



void setup() {
  pinMode(LED, OUTPUT);
  pinMode(breakBeam, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(breakBeam), ballDetected, CHANGE);
  pinMode(solenoid, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  
}

void ballDetected() {
  if(digitalRead(breakBeam)){
    digitalWrite(solenoid, LOW);
  }else{
    digitalWrite(solenoid, HIGH);
  }
}
