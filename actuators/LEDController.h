#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <Adafruit_NeoPixel.h>

class LEDController {
private:
    Adafruit_NeoPixel& strip;
    int numPixels;
    uint32_t currentColor;
    float brightness;
    bool isOn;
    
public:
    LEDController(Adafruit_NeoPixel& stripRef, int numPixels) 
        : strip(stripRef), numPixels(numPixels), brightness(1.0), isOn(false) {}
    
    void begin() {
        strip.begin();
        strip.show();
    }
    
    void setColor(uint8_t r, uint8_t g, uint8_t b) {
        currentColor = strip.Color(r, g, b);
        updateStrip();
    }
    
    void setBrightness(float value) {
        brightness = constrain(value, 0.0, 1.0);
        updateStrip();
    }
    
    void turnOn() {
        isOn = true;
        updateStrip();
    }
    
    void turnOff() {
        isOn = false;
        updateStrip();
    }
    
    void setBreathingEffect(uint8_t r, uint8_t g, uint8_t b, int duration) {
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
    
    void setPulseEffect(uint8_t r, uint8_t g, uint8_t b, int duration) {
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
    
private:
    void updateStrip() {
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
};

#endif 