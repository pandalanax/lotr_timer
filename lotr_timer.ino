#include "HT1621.h"
#include <Tone.h>

// Pin Definitions
#define BUZZER_PIN 12
#define DATA_PIN 11 // Display DATA -> PB3
#define WR_PIN   13 // Display WR   -> PB5
#define CS_PIN   10 // Display CS   -> PB2

#define MINUTE_BUTTON_PIN 2
#define SECOND_BUTTON_PIN 3
#define START_STOP_BUTTON_PIN 4

// Object Instances
Tone buzzer;
HT1621 display(DATA_PIN, WR_PIN, CS_PIN);

// RTTTL Song Data
char song[] = "LordOfTh:d=4,o=6,b=160:2a5,2g5,8g5,8g5,1a5,8d,8e,2f,8e,8d,2c,8d,8e,2d.,2c,b5,2a5,2g5,8g5,8g5,1a5,8d,8e,2f,8e,8d,2c,d,2e.,2e.,e";

// Timer variables
int minutes = 0;
int seconds = 0;
bool timerRunning = false;
unsigned long lastTick = 0;

// Button state management
int lastMinuteButtonState = HIGH;
int lastSecondButtonState = HIGH;
int lastStartStopButtonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void setup() {
  // --- Pin Modes ---
  pinMode(MINUTE_BUTTON_PIN, INPUT);
  pinMode(SECOND_BUTTON_PIN, INPUT);
  pinMode(START_STOP_BUTTON_PIN, INPUT);
  
  // --- Display ---
  display.begin();
  display.clear();
  updateDisplay();

  // --- Buzzer ---
  buzzer.begin(BUZZER_PIN);
}

void loop() {
  handleButtons();

  if (timerRunning) {
    if (millis() - lastTick >= 1000) {
      lastTick = millis();
      if (seconds > 0) {
        seconds--;
      } else if (minutes > 0) {
        minutes--;
        seconds = 59;
      } else {
        timerRunning = false;
        playRTTTL(song);
      }
      updateDisplay();
    }
  }
}

void updateDisplay() {
  int m1 = minutes / 10;
  int m2 = minutes % 10;
  int s1 = seconds / 10;
  int s2 = seconds % 10;

  display.writeDigit(3, m1);
  display.writeDigit(2, m2);
  display.writeDigit(1, s1);
  display.writeDigit(0, s2, true);
}

void handleButtons() {
  // Read the current state of the buttons
  int minuteReading = digitalRead(MINUTE_BUTTON_PIN);
  int secondReading = digitalRead(SECOND_BUTTON_PIN);
  int startStopReading = digitalRead(START_STOP_BUTTON_PIN);

  // Check if the button state has changed and enough time has passed to ignore noise
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // --- Handle Minute Button ---
    // A press is detected on the transition from HIGH to LOW
    if (minuteReading == LOW && lastMinuteButtonState == HIGH) {
      if (!timerRunning) {
        minutes = (minutes + 1) % 60;
        updateDisplay();
      }
      lastDebounceTime = millis(); // Reset debounce timer
    }

    // --- Handle Second Button ---
    if (secondReading == LOW && lastSecondButtonState == HIGH) {
      if (!timerRunning) {
        seconds = (seconds + 1) % 60;
        updateDisplay();
      }
      lastDebounceTime = millis(); // Reset debounce timer
    }

    // --- Handle Start/Stop Button ---
    if (startStopReading == LOW && lastStartStopButtonState == HIGH) {
      timerRunning = !timerRunning;
      if (timerRunning) {
        lastTick = millis();
      }
      lastDebounceTime = millis(); // Reset debounce timer
    }
  }

  // Save the current button states for the next loop
  lastMinuteButtonState = minuteReading;
  lastSecondButtonState = secondReading;
  lastStartStopButtonState = startStopReading;
}

// --- RTTTL Player Functions ---

void playRTTTL(char *p) {
  int default_dur = 4;
  int default_oct = 6;
  int bpm = 63;
  long wholenote = 0;
  int num = 0;
  
  while (*p != ':') p++;
  p++;
  
  if (*p == 'd') {
    p += 2;
    num = 0;
    while (isdigit(*p)) {
      num = num * 10 + (*p++ - '0');
    }
    if (num) default_dur = num;
    if (*p == ',') p++;
  }
  
  if (*p == 'o') {
    p += 2;
    default_oct = *p++ - '0';
    if (*p == ',') p++;
  }
  
  if (*p == 'b') {
    p += 2;
    num = 0;
    while (isdigit(*p)) {
      num = num * 10 + (*p++ - '0');
    }
    bpm = num;
    if (*p == ':') p++;
  }
  
  wholenote = (60 * 1000L / bpm) * 4;
  
  while (*p) {
    num = 0;
    while (isdigit(*p)) {
      num = num * 10 + (*p++ - '0');
    }
    long duration = wholenote / (num ? num : default_dur);
    
    int note = 0;
    switch (*p) {
      case 'c': note = 1; break;
      case 'd': note = 3; break;
      case 'e': note = 5; break;
      case 'f': note = 6; break;
      case 'g': note = 8; break;
      case 'a': note = 10; break;
      case 'b': note = 12; break;
      case 'p': note = 0; break;
    }
    p++;
    
    if (*p == '#') {
      note++;
      p++;
    }
    
    if (*p == '.') {
      duration += duration / 2;
      p++;
    }
    
    int scale = default_oct;
    if (isdigit(*p)) {
      scale = *p - '0';
      p++;
    }
    
    if (*p == ',') p++;
    
    if (note) {
      int freq = noteFrequency(note, scale);
      buzzer.play(freq, duration);
      delay(duration);
    } else {
      delay(duration);
    }
  }
}

int noteFrequency(int note, int octave) {
  static const int baseNotes[] = {
    0,
    262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494
  };
  int freq = baseNotes[note];
  if (octave > 4) {
    freq <<= (octave - 4);
  } else if (octave < 4) {
    freq >>= (4 - octave);
  }
  return freq;
}