#include <SoftwareSerial.h>
#include "notes.h"

SoftwareSerial mySerial(2, 3); // RX, TX
#define breakBeam 5
#define LED 13
#define solenoid 6


byte resetMIDI = 4; //Tied to VS1053 Reset line

// DEVELOPING THE SONG //
int beatPeriod = 500; // 120 BPM = 500 ms   NOTE_TO_SELF: Change this to 1/16th beat (or 1/32nd if used)
int instrument = 73; // Starts at 0
float song[2][15] = 
{
  {A_5  ,Fs_5   ,B_5  ,E_5  ,E_5  ,Fs_5 ,G_5  ,E_5  ,A_5  ,Fs_5 ,E_5  ,D_5    ,C_5    ,As_4 ,B_4},
  {2    ,1      ,1    ,3    ,0.5  ,0.5  ,2    ,1    ,1    ,0.5  ,0.5  ,0.5    ,0.5    ,1    ,1  }
};
int noteNum = 0;
int songLength = 15;

// OTHER IMPORTANT VARIABLES //
boolean solenoidFound = false;
boolean readyForSong = true;
unsigned long initTime = 0;
int initDelay = 500;
unsigned long nextNoteTime = 0;
unsigned long solenoidDropTime = 0;
int holdTime = 2000;



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
