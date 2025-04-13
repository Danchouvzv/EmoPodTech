#include "LEDController.h"

void LEDController::begin() {
    strip.begin();
    strip.show();
}

void LEDController::setColor(uint8_t r, uint8_t g, uint8_t b) {
    currentColor = strip.Color(r, g, b);
    updateStrip();
}

void LEDController::setBrightness(float value) {
    brightness = constrain(value, 0.0, 1.0);
    updateStrip();
}

void LEDController::turnOn() {
    isOn = true;
    updateStrip();
}

void LEDController::turnOff() {
    isOn = false;
    updateStrip();
}

void LEDController::setBreathingEffect(uint8_t r, uint8_t g, uint8_t b, int duration) {
    uint32_t color = strip.Color(r, g, b);
    for (int i = 0; i < 256; i++) {
        float brightness = (exp(sin(i/256.0*PI)) - 0.36787944) * 108.0/255.0;
        strip.setBrightness(brightness * 255);
        for (int j = 0; j < numPixels; j++) {
            strip.setPixelColor(j, color);
        }
        strip.show();
        delay(duration / 256);
    }
}

void LEDController::setPulseEffect(uint8_t r, uint8_t g, uint8_t b, int duration) {
    uint32_t color = strip.Color(r, g, b);
    for (int i = 0; i < numPixels; i++) {
        strip.setPixelColor(i, color);
        strip.show();
        delay(duration / numPixels);
    }
    for (int i = numPixels - 1; i >= 0; i--) {
        strip.setPixelColor(i, 0);
        strip.show();
        delay(duration / numPixels);
    }
}

void LEDController::updateStrip() {
    if (isOn) {
        strip.setBrightness(brightness * 255);
        for (int i = 0; i < numPixels; i++) {
            strip.setPixelColor(i, currentColor);
        }
    } else {
        for (int i = 0; i < numPixels; i++) {
            strip.setPixelColor(i, 0);
        }
    }
    strip.show();
} 