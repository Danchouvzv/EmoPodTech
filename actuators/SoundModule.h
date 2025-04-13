#ifndef SOUND_MODULE_H
#define SOUND_MODULE_H

#include <Arduino.h>

class SoundModule {
private:
    int pin;
    bool isPlaying;
    unsigned long startTime;
    int currentNote;
    int currentDuration;
    
    // Meditation tones frequencies (Hz)
    const int TONES[8] = {
        196,  // G3
        220,  // A3
        247,  // B3
        262,  // C4
        294,  // D4
        330,  // E4
        349,  // F4
        392   // G4
    };
    
    // Breathing exercise pattern
    const int BREATHING_PATTERN[4] = {
        4,  // Inhale (4 seconds)
        2,  // Hold (2 seconds)
        6,  // Exhale (6 seconds)
        2   // Pause (2 seconds)
    };
    
public:
    SoundModule(int pin) : pin(pin), isPlaying(false), startTime(0), currentNote(0), currentDuration(0) {}
    
    void begin() {
        pinMode(pin, OUTPUT);
    }
    
    void playMeditationTone(int toneIndex, int duration) {
        if (toneIndex >= 0 && toneIndex < 8) {
            tone(pin, TONES[toneIndex], duration);
        }
    }
    
    void startBreathingExercise() {
        isPlaying = true;
        startTime = millis();
        currentNote = 0;
        currentDuration = BREATHING_PATTERN[0] * 1000;
        tone(pin, TONES[0], currentDuration);
    }
    
    void update() {
        if (isPlaying) {
            unsigned long currentTime = millis();
            if (currentTime - startTime >= currentDuration) {
                currentNote = (currentNote + 1) % 4;
                currentDuration = BREATHING_PATTERN[currentNote] * 1000;
                startTime = currentTime;
                tone(pin, TONES[currentNote], currentDuration);
            }
        }
    }
    
    void stop() {
        isPlaying = false;
        noTone(pin);
    }
    
    void playSuccessTone() {
        tone(pin, TONES[3], 200);
        delay(200);
        tone(pin, TONES[5], 200);
        delay(200);
        tone(pin, TONES[7], 400);
        delay(400);
        noTone(pin);
    }
    
    void playErrorTone() {
        tone(pin, TONES[7], 100);
        delay(100);
        tone(pin, TONES[3], 300);
        delay(300);
        noTone(pin);
    }
};

#endif 