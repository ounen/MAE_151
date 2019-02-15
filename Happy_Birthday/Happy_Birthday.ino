#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX

byte note = 0; //The MIDI note value to be played
byte resetMIDI = 4; //Tied to VS1053 Reset line
byte ledPin = 13; //MIDI traffic inidicator
int  instrument = 96; // Starts at 0

int G = 67;
int G2 = 79;
int A = 69;
int C = 72;
int B = 71;
int D = 74;
int E = 76;
int F = 77;
int song[] = {G, G, A, G, C, B, 
              G, G, A, G, D, C, 
              G, G, G2, E, C, B, A, 
              F, F, E, C, D, C};

void setup() {
  Serial.begin(57600);

  //Setup soft serial for MIDI control
  mySerial.begin(31250);

  //Reset the VS1053
  pinMode(resetMIDI, OUTPUT);
  digitalWrite(resetMIDI, LOW);
  delay(100);
  digitalWrite(resetMIDI, HIGH);
  delay(100);
  talkMIDI(0xB0, 0x07, 100); //0xB0 is channel message, set channel volume to near max (127)
}

void loop() {
  //Demo Basic MIDI instruments, GM1
  Serial.println("Basic Instruments");
  talkMIDI(0xB0, 0, 0x00); //Default bank GM1
  talkMIDI(0xB0, 1, 0x00); //Default bank GM1

 

  talkMIDI(0xC0, instrument, 0); //Set instrument number. 0xC0 is a 1 data byte command
  talkMIDI(0xC0, 0, 1); //TEST
  //Play notes from F#-0 (30) to F#-5 (90):
  for (int i = 0 ; i < 25 ; i++) {
    Serial.print("N:");
    Serial.println(song[i], DEC);
    
    //Note on channel 1 (0x90), some note value (note), middle velocity (0x45):
    noteOn(0, song[i], 60);
    noteOn(1, song[i], 60); // TEST
    delay(50);

    //Turn off the note with a given off/release velocity
    noteOff(0, song[i], 60);
    noteOn(1, song[i], 60); // TEST
    delay(200);
  }
}





//Send a MIDI note-on message.  Like pressing a piano key
//channel ranges from 0-15
void noteOn(byte channel, byte note, byte attack_velocity) {
  talkMIDI( (0x90 | channel), note, attack_velocity);
}

//Send a MIDI note-off message.  Like releasing a piano key
void noteOff(byte channel, byte note, byte release_velocity) {
  talkMIDI( (0x80 | channel), note, release_velocity);
}

//Plays a MIDI note. Doesn't check to see that cmd is greater than 127, or that data values are less than 127
void talkMIDI(byte cmd, byte data1, byte data2) {
  digitalWrite(ledPin, HIGH);
  mySerial.write(cmd);
  mySerial.write(data1);

  //Some commands only have one data byte. All cmds less than 0xBn have 2 data bytes 
  //(sort of: http://253.ccarh.org/handout/midiprotocol/)
  if( (cmd & 0xF0) <= 0xB0)
    mySerial.write(data2);

  digitalWrite(ledPin, LOW);
}
