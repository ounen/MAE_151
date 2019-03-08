#include <SoftwareSerial.h>
#include "notes.h" // All MIDI notes converted to sheet music notes

SoftwareSerial mySerial(2, 3); // RX, TX
#define breakBeam 5
#define LED 13
#define solenoid 6
#define red 9
#define green 10
#define blue 11


// MIDI AND SONG DEVELOPEMENT //
byte resetMIDI = 4; //Tied to VS1053 Reset line
int beatPeriod = 500; // 120 BPM = 500 ms   
int instrument = 73; // Starts at 0
int songLength = 4;
float song[2][4] = 
{
  {E_4, C_4, B_3, A_3},
  {2, 0.5, 0.5, 1}
};
int noteNum = 0; // Tracker for Arduino to know what note to play
int color = 0;

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
  talkMIDI(0xB0, 0x07, 115); //0xB0 is channel message, set channel volume to near max (127)
  talkMIDI(0xB0, 0, 0x00); //Default bank GM1
  talkMIDI(0xC0, instrument, 0); //Set instrument number. 0xC0 is a 1 data byte command

  // LED LIGHT INITIALIZATION //
  analogWrite(red, 0);
  analogWrite(green, 0);
  analogWrite(blue, 0);

  // SENSOR/ACTUATOR INITIALIZATION //
  pinMode(breakBeam, INPUT_PULLUP);
  pinMode(solenoid, OUTPUT);

  // SERIAL COMMUNICATION //
  Serial.begin(9600);
  
}

void loop() {

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
    
    while( noteNum <= songLength ){
      
      // MIDI: WAIT FOR NEXT NOTE TO PLAY
      if( millis() > nextNoteTime){
        Serial.println(song[0][noteNum], DEC);
        
        noteOff(0, song[0][noteNum - 1], 60); // Turn off previous note
        // If not a null note, play the note
        if( song[0][noteNum] != -1 ){
          noteOn(0, song[0][noteNum], 60);

     
          analogWrite(blue, 180);
        }
        
        nextNoteTime += beatPeriod*song[1][noteNum]; // Define when to play next note
        noteNum++; // Move to next note in array
        color++;
      }

      // SOLENOID: CHECK IF IT'S TIME TO DROP THE BALL
      if( millis() > solenoidDropTime ){
        solenoidFound = false;
        digitalWrite(solenoid, LOW);
        readyForSong = true;
        
        Serial.println("Release Time");
        analogWrite(red, 0);
        analogWrite(green, 0);
        analogWrite(blue, 0);
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

// LED Basic //
void playLED(int count){
  if (count == -1){
    analogWrite(red, 0);
    analogWrite(green, 0);
    analogWrite(blue, 0);
  }else{
    switch (count%2){
      case 0:
        analogWrite(red, 200);
        analogWrite(green, 0);
        analogWrite(blue, 0);
      case 1:
        analogWrite(red, 0);
        analogWrite(green, 200);
        analogWrite(blue, 0);
      case 2:
        analogWrite(red, 0);
        analogWrite(green, 0);
        analogWrite(blue, 200);
    }
  }
}

// MIDI MUSIC PLAYING //

// Send a MIDI note-on message.  Like pressing a piano key
// channel ranges from 0-15
void noteOn(byte channel, byte note, byte attack_velocity) {
  talkMIDI( (0x90 | channel), note, attack_velocity);

}

// Send a MIDI note-off message.  Like releasing a piano key
void noteOff(byte channel, byte note, byte release_velocity) {
  talkMIDI( (0x80 | channel), note, release_velocity);
}

// Plays a MIDI note. Doesn't check to see that cmd is greater than 127, or that data values are less than 127
void talkMIDI(byte cmd, byte data1, byte data2) {
  mySerial.write(cmd);
  mySerial.write(data1);

  //Some commands only have one data byte. All cmds less than 0xBn have 2 data bytes 
  //(sort of: http://253.ccarh.org/handout/midiprotocol/)
  if( (cmd & 0xF0) <= 0xB0)
    mySerial.write(data2);
}
