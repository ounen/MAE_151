#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); // RX, TX
#define breakBeam 5
#define LED 13
#define solenoid 6

byte resetMIDI = 4; //Tied to VS1053 Reset line

int C_n1 = 0,  Cs_n1 = 1,  D_n1 = 2,  Ds_n1 = 3,  E_n1 = 4,  F_n1 = 5,  Fs_n1 = 6,  G_n1 = 7,  Gs_n1 = 8,  A_n1 = 9,  As_n1 = 10, B_n1 = 11,
    C_0 = 12,  Cs_0 = 13,  D_0 = 14,  Ds_0 = 15,  E_0 = 16,  F_0 = 17,  Fs_0 = 18,  G_0 = 19,  Gs_0 = 20,  A_0 = 21,  As_0 = 22,  B_0 = 23,
    C_1 = 24,  Cs_1 = 25,  D_1 = 26,  Ds_1 = 27,  E_1 = 28,  F_1 = 29,  Fs_1 = 30,  G_1 = 31,  Gs_1 = 32,  A_1 = 33,  As_1 = 34,  B_1 = 35,
    C_2 = 36,  Cs_2 = 37,  D_2 = 38,  Ds_2 = 39,  E_2 = 40,  F_2 = 41,  Fs_2 = 42,  G_2 = 43,  Gs_2 = 44,  A_2 = 45,  As_2 = 46,  B_2 = 47,
    C_3 = 48,  Cs_3 = 49,  D_3 = 50,  Ds_3 = 51,  E_3 = 52,  F_3 = 53,  Fs_3 = 54,  G_3 = 55,  Gs_3 = 56,  A_3 = 57,  As_3 = 58,  B_3 = 59,
    C_4 = 60,  Cs_4 = 61,  D_4 = 62,  Ds_4 = 63,  E_4 = 64,  F_4 = 65,  Fs_4 = 66,  G_4 = 67,  Gs_4 = 68,  A_4 = 69,  As_4 = 70,  B_4 = 71,
    C_5 = 72,  Cs_5 = 73,  D_5 = 74,  Ds_5 = 75,  E_5 = 76,  F_5 = 77,  Fs_5 = 78,  G_5 = 79,  Gs_5 = 80,  A_5 = 81,  As_5 = 82,  B_5 = 83,
    C_6 = 84,  Cs_6 = 85,  D_6 = 86,  Ds_6 = 87,  E_6 = 88,  F_6 = 89,  Fs_6 = 90,  G_6 = 91,  Gs_6 = 92,  A_6 = 93,  As_6 = 94,  B_6 = 95,
    C_7 = 96,  Cs_7 = 97,  D_7 = 98,  Ds_7 = 99,  E_7 = 100, F_7 = 101, Fs_7 = 102, G_7 = 103, Gs_7 = 104, A_7 = 105, As_7 = 106, B_7 = 107,
    C_8 = 108, Cs_8 = 109, D_8 = 110, Ds_8 = 111, E_8 = 112, F_8 = 113, Fs_8 = 114, G_8 = 115, Gs_8 = 116, A_8 = 117, As_8 = 118, B_8 = 119,
    C_9 = 120, Cs_9 = 121, D_9 = 122, Ds_9 = 123, E_9 = 124, F_9 = 125, Fs_9 = 126, G_9 = 127;


// DEVELOPING THE SONG //
int beatPeriod = 500; // 120 BPM = 500 ms   NOTE_TO_SELF: Change this to 1/16th beat (or 1/32nd if used)
int instrument = 0; // Starts at 0
int song[2][14] = 
{
  {D_3, A_3, A_3, G_2, G_2, A_3, B_3, A_3, G_2, E_2, A_3, Fs_2, Cs_2, D_2},
  {1  , 1  , 1  , 1  , 3  , 1  , 1  , 1  , 1  , 1  , 1  , 1   , 1   , 1  }
};
int noteNum = 0;
int songLength = 14;

// OTHER IMPORTANT VARIABLES //
boolean solenoidFound = false;
boolean readyForSong = true;
int initTime = 0;
int initDelay = 500;
int nextNoteTime = 0;
int solenoidDropTime = 0;
int holdTime = 8000;



void setup() {
  // MIDI INITIALIZATION //
  mySerial.begin(31250);
  pinMode(resetMIDI, OUTPUT);
  digitalWrite(resetMIDI, LOW);
  delay(100);
  digitalWrite(resetMIDI, HIGH);
  delay(100);
  talkMIDI(0xB0, 0x07, 120); //0xB0 is channel message, set channel volume to near max (127)


  // OTHER INITIALIZATION //
  pinMode(LED, OUTPUT);
  pinMode(breakBeam, INPUT_PULLUP);
  pinMode(solenoid, OUTPUT);
  
  Serial.begin(9600);
}

void loop() {
  talkMIDI(0xB0, 0, 0x00); //Default bank GM1
  talkMIDI(0xC0, instrument, 0); //Set instrument number. 0xC0 is a 1 data byte command

  if( digitalRead(breakBeam) == false ){
    digitalWrite(solenoid, HIGH);
    solenoidFound = true;
  }
  
  if( solenoidFound && readyForSong ){
    Serial.println("Ball is found");
    Serial.println(song[0][noteNum], DEC);
    readyForSong = false;
    
    // Begin Timer, Play First Note, Set Time of Next Note
    initTime = millis();
    solenoidDropTime = initTime + holdTime;
    nextNoteTime = initTime + initDelay;
    //noteOn(0, song[0][noteNum], 60); //Don't use if planning for initial delay
    //nextNoteTime = initTime + beatPeriod*song[1][noteNum]; //Don't use if planning for initial delay
    //noteNum++; //Don't use if planning for initial delay
    
    while( noteNum <= songLength ){
      
      // MIDI: WAIT FOR NEXT NOTE TO PLAY
      if( millis() > nextNoteTime){
        Serial.println(song[0][noteNum], DEC);
        
        noteOff(0, song[0][noteNum - 1], 60); // Turn off previous note
        // If not a null note, play the note
        if( song[0][noteNum] != -1 ){
          noteOn(0, song[0][noteNum], 60);
        }
        nextNoteTime += beatPeriod*song[1][noteNum];

        noteNum++;  
      }

      // SOLENOID: CHECK IF IT'S TIME TO DROP THE BALL
      if( millis() > solenoidDropTime ){
        solenoidFound = false;
        digitalWrite(solenoid, LOW);
        readyForSong = true;
        
        Serial.println("Release Time");
      }
    }

    noteOff(0, song[0][noteNum - 1], 60); // Turn off previous note
    noteNum = 0;
  }

  // SOLENOID: CHECK IF IT'S TIME TO DROP THE BALL (IN CASE DROP TIME IS AFTER SONG HAS PLAYED)
  if( millis() > solenoidDropTime && solenoidDropTime != 0 ){
    solenoidFound = false;
    digitalWrite(solenoid, LOW);
    readyForSong = true;
    
    Serial.println("Release Time");
  }
  
}

//////////////////////////
// ADDITIONAL FUNCTIONS //
//////////////////////////

// MIDI MUSIC PLAYING //

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
  mySerial.write(cmd);
  mySerial.write(data1);

  //Some commands only have one data byte. All cmds less than 0xBn have 2 data bytes 
  //(sort of: http://253.ccarh.org/handout/midiprotocol/)
  if( (cmd & 0xF0) <= 0xB0)
    mySerial.write(data2);
}
