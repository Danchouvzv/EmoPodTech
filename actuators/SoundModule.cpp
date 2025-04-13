#include "SoundModule.h"

void SoundModule::begin() {
    pinMode(pin, OUTPUT);
}

void SoundModule::playMeditationTone(int toneIndex, int duration) {
    if (toneIndex >= 0 && toneIndex < 8) {
        tone(pin, TONES[toneIndex], duration);
    }
}

void SoundModule::startBreathingExercise() {
    isPlaying = true;
    startTime = millis();
    currentNote = 0;
    currentDuration = BREATHING_PATTERN[0] * 1000;
    tone(pin, TONES[0], currentDuration);
}

void SoundModule::update() {
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

void SoundModule::stop() {
    isPlaying = false;
    noTone(pin);
}

void SoundModule::playSuccessTone() {
    tone(pin, TONES[3], 200);
    delay(200);
    tone(pin, TONES[5], 200);
    delay(200);
    tone(pin, TONES[7], 400);
    delay(400);
    noTone(pin);
}

void SoundModule::playErrorTone() {
    tone(pin, TONES[7], 100);
    delay(100);
    tone(pin, TONES[3], 300);
    delay(300);
    noTone(pin);
} 