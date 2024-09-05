#include "pitches.h"

const int buzzerPin = 9;
const int buttonPin = 12;
const int redPin = 7;
const int yellowPin = 6;
const int greenPin = 5;

const int playedNoteDuration = 100; // how long each note is actually played for

const int numIntroBeats = 16;
const int introMelody[numIntroBeats] = {NOTE_C3, -1, -1, NOTE_C3, NOTE_C3, -1, -1, NOTE_C3, NOTE_C3, -1, -1, NOTE_C3, -1, NOTE_C3, -1, NOTE_G2};

const int numPhrasesPerCycle = 4;
const int numMelodyBeats = 14;
const int melody[numPhrasesPerCycle][numMelodyBeats] = { // separated by phrases
  {NOTE_C3, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_E4, -1, NOTE_C4},
  {NOTE_C3, NOTE_GS3, NOTE_GS3, NOTE_GS3, NOTE_GS3, NOTE_GS3, NOTE_AS3, NOTE_GS3, NOTE_G3, -1, NOTE_A3, NOTE_B3, -1, -1},
  {NOTE_C3, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_E4, -1, NOTE_C4},
  {NOTE_C4, NOTE_C4, NOTE_C4, -1, NOTE_D4, NOTE_C4, NOTE_B3, NOTE_C4, NOTE_C3, -1, NOTE_G3, -1, NOTE_A3, -1}
};
const int numTotalPhrases = 12;
const int phraseTempos[numTotalPhrases] = {120, 120, 160, 120, 120, 80, 160, 160, 80, 80, 80, 120};
const int phraseFadeouts[numTotalPhrases] = {14, 12, 14, 14, 6, 14, 10, 5, 14, 11, 5, 14};

// phrase level
int phraseIndex;
int tempo;
int beatInterval;  // interval between beats in ms
int noteDurationLong;  // first swing note
int noteDurationShort; // second swing note
int fadeout; // beat at which melody stops playing
int melodyIndex;
int perfectTolerance; // tolerance in ms for "perfect"
int barelyTolerance; // tolerance in ms for "barely"

// note level
int noteIndex;
int noteDuration; // includes the silence after the staccato (playedNoteDuration)

unsigned long previousNoteMillis = 0;
unsigned long currentMillis = 0;

void setup() {
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(redPin, OUTPUT);
}

void loop() {
  phraseIndex = 0;
  setupPhrase(phraseIndex);

  // play intro
  while (noteIndex < numIntroBeats) {
    noteDuration = noteIndex % 2 == 0 ? noteDurationLong : noteDurationShort;
    if (introMelody[noteIndex] > 0)
      playNote(introMelody, noteIndex, noteDuration);
    else
      delay(noteDuration);
    noteIndex++;
  }

  noteIndex = 0;
  noteDuration = noteDurationLong;
  
  // play melody
  while (phraseIndex < numTotalPhrases) {
    currentMillis = millis();

    if (currentMillis - previousNoteMillis >= noteDuration) { // marks each note
      previousNoteMillis = currentMillis;
      noteDuration = noteIndex % 2 == 0 ? noteDurationLong : noteDurationShort;

      if (noteIndex <= fadeout && melody[melodyIndex][noteIndex] > 0)
        playNote(melody[melodyIndex], noteIndex, noteDuration);
      else
        delay(noteDuration);

      noteIndex++;
      
      if (noteIndex == numMelodyBeats) { // marks end of each phrase
        // wait for button press
        unsigned long pressStart = millis();
        bool buttonPressed = false;
        while (millis() - pressStart < 2 * beatInterval) {
          if (digitalRead(buttonPin) == HIGH) {
            if (millis() - pressStart <= perfectTolerance)
              blinkLED(greenPin);
            else if (millis() - pressStart <= barelyTolerance)
              blinkLED(yellowPin);
            else
              blinkLED(redPin);
            buttonPressed = true;
            break;
          }
        }
        if (!buttonPressed)
          blinkLED(redPin);
        delay(2000);

        phraseIndex++;
        if (phraseIndex < numTotalPhrases) setupPhrase(phraseIndex);
      }
    }
  }
  delay(5000);
}

void setupPhrase(int phraseIndex) {
  melodyIndex = phraseIndex % 4;
  tempo = phraseTempos[phraseIndex];
  beatInterval = (60.0 / tempo) * 1000;
  noteDurationLong = beatInterval * 0.667;
  noteDurationShort = beatInterval * 0.333;
  fadeout = phraseFadeouts[phraseIndex];
  perfectTolerance = noteDurationShort;
  barelyTolerance = noteDurationLong;

  noteIndex = 0;
  noteDuration = noteDurationLong;
}

void blinkLED(int pin) {
  digitalWrite(pin, HIGH);
  delay(500);
  digitalWrite(pin, LOW);
}

void playNote(int melodyArr[], int index, int duration) {
  tone(buzzerPin, melodyArr[index]);
  delay(playedNoteDuration);
  noTone(buzzerPin);
  delay(duration - playedNoteDuration);
}
