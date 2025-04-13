#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

class NetworkManager {
private:
    const char* ssid;
    const char* password;
    const char* serverUrl;
    
    unsigned long lastReconnectAttempt;
    const unsigned long RECONNECT_INTERVAL = 5000; // 5 seconds
    
    bool isConnected;
    int failedAttempts;
    const int MAX_FAILED_ATTEMPTS = 3;
    
public:
    NetworkManager(const char* ssid, const char* password, const char* serverUrl)
        : ssid(ssid), password(password), serverUrl(serverUrl),
          lastReconnectAttempt(0), isConnected(false), failedAttempts(0) {}
    
    void begin() {
        WiFi.mode(WIFI_STA);
        connect();
    }
    
    void update() {
        if (WiFi.status() != WL_CONNECTED) {
            unsigned long currentMillis = millis();
            if (currentMillis - lastReconnectAttempt >= RECONNECT_INTERVAL) {
                lastReconnectAttempt = currentMillis;
                connect();
            }
        }
    }
    
    bool sendData(const JsonDocument& doc) {
        if (WiFi.status() != WL_CONNECTED) {
            return false;
        }
        
        HTTPClient http;
        http.begin(serverUrl);
        http.addHeader("Content-Type", "application/json");
        
        String jsonString;
        serializeJson(doc, jsonString);
        
        int httpResponseCode = http.POST(jsonString);
        bool success = (httpResponseCode > 0);
        
        if (!success) {
            failedAttempts++;
            if (failedAttempts >= MAX_FAILED_ATTEMPTS) {
                // Reset connection after too many failures
                WiFi.disconnect();
                isConnected = false;
                failedAttempts = 0;
            }
        } else {
            failedAttempts = 0;
        }
        
        http.end();
        return success;
    }
    
    bool isWiFiConnected() const {
        return isConnected;
    }
    
private:
    void connect() {
        Serial.println("Connecting to WiFi...");
        WiFi.begin(ssid, password);
        
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
            delay(500);
            Serial.print(".");
            attempts++;
        }
        
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\nConnected to WiFi");
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());
            isConnected = true;
        } else {
            Serial.println("\nFailed to connect to WiFi");
            isConnected = false;
        }
    }
};

#endif 