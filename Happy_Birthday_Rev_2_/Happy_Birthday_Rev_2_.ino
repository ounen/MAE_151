#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX

byte note = 0; //The MIDI note value to be played
byte resetMIDI = 4; //Tied to VS1053 Reset line
byte ledPin = 13; //MIDI traffic inidicator
int  instrument = 93; // Starts at 0

int beatPeriod = 500; // 120 BPM = 500 ms 

int G_4 = 67;
int G_5 = 79;
int A_4 = 69;
int C_5 = 72;
int B_4 = 71;
int D_5 = 74;
int E_5 = 76;
int F_5 = 77;
int song[2][25] = 
{
  {G_4  ,G_4  ,A_4  ,G_4  ,C_5  ,B_4  ,G_4  ,G_4  ,A_4  ,G_4  ,D_5  ,C_5  ,G_4  ,G_4  ,G_5  ,E_5  ,C_5  ,B_4  ,A_4  ,F_5  ,F_5  ,E_5  ,C_5  ,D_5  ,C_5},
  {1    ,1    ,1    ,1    ,1    ,2    ,1    ,1    ,1    ,1    ,1    ,2    ,1    ,1    ,1    ,1    ,1    ,1    ,1    ,1    ,1    ,1    ,1    ,1    ,2  } 
};


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
  talkMIDI(0xB0, 0x07, 50); //0xB0 is channel message, set channel volume to near max (127)
}

void loop() {
  //Demo Basic MIDI instruments, GM1
  Serial.println("Basic Instruments");
  talkMIDI(0xB0, 0, 0x00); //Default bank GM1
 
  talkMIDI(0xC0, instrument, 0); //Set instrument number. 0xC0 is a 1 data byte command
  //Play notes from F#-0 (30) to F#-5 (90):
  for (int i = 0 ; i < 25 ; i++) {
    Serial.print("N:");
    Serial.println(song[0][i], DEC);
    
    //Note on channel 1 (0x90), some note value (note), middle velocity (0x45):
    noteOn(0, song[0][i]-24, 60);
    noteOn(0, song[0][i], 60);
    delay(beatPeriod*song[1][i]);

    //Turn off the note with a given off/release velocity
    noteOff(0, song[0][i]-24, 60);
    noteOff(0, song[0][i], 60);
    delay(0);
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
