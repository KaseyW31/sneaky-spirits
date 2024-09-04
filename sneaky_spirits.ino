#include "pitches.h"

const int buzzerPin = 9;
const int buttonPin = 12;
const int redPin = 7;
const int yellowPin = 6;
const int greenPin = 5;

const int introMelody[16] = {NOTE_C3, -1, -1, NOTE_C3, NOTE_C3, -1, -1, NOTE_C3, NOTE_C3, -1, -1, NOTE_C3, -1, NOTE_C3, -1, NOTE_G2};

const int melody[4][14] = { // separated by phrases
  {NOTE_C3, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_E4, -1, NOTE_C4},
  {NOTE_C3, NOTE_GS3, NOTE_GS3, NOTE_GS3, NOTE_GS3, NOTE_GS3, NOTE_AS3, NOTE_GS3, NOTE_G3, -1, NOTE_A3, NOTE_B3, -1, -1},
  {NOTE_C3, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_E4, -1, NOTE_C4},
  {NOTE_C4, NOTE_C4, NOTE_C4, -1, NOTE_D4, NOTE_C4, NOTE_B3, NOTE_C4, NOTE_C3, -1, NOTE_G3, -1, NOTE_A3, -1}
};
const int phraseTempos[12] = {120, 120, 160, 120, 120, 80, 160, 160, 80, 80, 80, 120};
const int phraseFadeouts[12] = {14, 12, 14, 14, 6, 14, 10, 5, 14, 11, 5, 14};

// phrase level
int phraseIndex;
int tempo;
int beatInterval;  // interval between beats in ms
int noteDurationOdd;  // first swing note
int noteDurationEven; // second swing note
int fadeout; // beat at which melody stops playing
int melodyIndex;
int perfectTolerance; // tolerance in ms for "perfect"
int barelyTolerance; // tolerance in ms for "barely"

// note level
int noteIndex;
int noteDuration;

unsigned long previousNoteMillis = millis();
unsigned long currentMillis = millis();

void setup() {
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(redPin, OUTPUT);
}

void loop() {
  phraseIndex = 0;
  setupPhrase(phraseIndex);

  // play intro
  while (noteIndex < 16) {
    if (introMelody[noteIndex] > 0) tone(buzzerPin, introMelody[noteIndex]);
    noteDuration = noteIndex % 2 == 0 ? noteDurationOdd : noteDurationEven;
    delay(100);
    noTone(buzzerPin);
    delay(noteDuration - 100);
    noteIndex++;
  }

  noteIndex = 0;
  noteDuration = noteDurationOdd;
  
  // play melody
  while (phraseIndex < 12) {
    currentMillis = millis();

    if (currentMillis - previousNoteMillis >= noteDuration) { // marks each note
      previousNoteMillis = currentMillis;
      if (noteIndex <= fadeout && melody[melodyIndex][noteIndex] > 0) tone(buzzerPin, melody[melodyIndex][noteIndex]);
      noteDuration = noteIndex % 2 == 0 ? noteDurationOdd : noteDurationEven;

      delay(100);
      noTone(buzzerPin);
      if (noteDuration > 100) delay(noteDuration - 100);

      noteIndex++;
      
      if (noteIndex == 14) { // marks end of each phrase
        // wait for button press
        unsigned long pressStart = millis();
        bool buttonPressed = false;
        while (millis() - pressStart < 2 * beatInterval) {
          if (digitalRead(buttonPin) == HIGH) {
            if (millis() - pressStart <= perfectTolerance) blinkLED(greenPin);
            else if (millis() - pressStart <= barelyTolerance) blinkLED(yellowPin);
            else blinkLED(redPin);
            buttonPressed = true;
            break;
          }
        }
        if (!buttonPressed) blinkLED(redPin);
        delay(2000);

        phraseIndex++;
        if (phraseIndex < 12) setupPhrase(phraseIndex);
      }
    }
  }
  delay(5000);
}

void setupPhrase(int phraseIndex) {
  melodyIndex = phraseIndex % 4;
  tempo = phraseTempos[phraseIndex];
  beatInterval = (60.0 / tempo) * 1000;
  noteDurationOdd = beatInterval * 0.667;
  noteDurationEven = beatInterval * 0.333;
  fadeout = phraseFadeouts[phraseIndex];
  perfectTolerance = noteDurationEven;
  barelyTolerance = noteDurationOdd;

  noteIndex = 0;
  noteDuration = noteDurationOdd;
}

void blinkLED(int pin) {
  digitalWrite(pin, HIGH);
  delay(500);
  digitalWrite(pin, LOW);
}