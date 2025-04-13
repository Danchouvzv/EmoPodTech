#include "NetworkManager.h"
#include "utils/Logger.h"
#include "utils/DataBuffer.h"

namespace Emopod {
namespace Network {

NetworkManager::NetworkManager(const char* ssid, const char* password, const char* serverUrl)
    : ssid(ssid), password(password), serverUrl(serverUrl),
      lastReconnectAttempt(0), failedAttempts(0), isConnected(false),
      dataBuffer(new Utils::DataBuffer()) {}

NetworkManager::~NetworkManager() {
    delete dataBuffer;
}

void NetworkManager::begin() {
    WiFi.mode(WIFI_STA);
    connect();
}

void NetworkManager::update() {
    if (WiFi.status() != WL_CONNECTED) {
        unsigned long currentMillis = millis();
        if (currentMillis - lastReconnectAttempt >= getReconnectDelay()) {
            lastReconnectAttempt = currentMillis;
            connect();
        }
    } else {
        // Try to send any buffered data
        sendBufferedData();
    }
}

bool NetworkManager::sendData(const JsonDocument& doc) {
    if (WiFi.status() != WL_CONNECTED) {
        Logger::warn("NETWORK", "WiFi not connected, buffering data");
        return dataBuffer->addData(doc);
    }
    
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(5000); // 5 second timeout
    
    String jsonString;
    if (serializeJson(doc, jsonString) == 0) {
        Logger::error("NETWORK", "Failed to serialize JSON");
        return false;
    }
    
    Logger::debug("NETWORK", "Sending data to %s", serverUrl);
    int httpResponseCode = http.POST(jsonString);
    
    if (httpResponseCode > 0) {
        String response = http.getString();
        Logger::info("NETWORK", "Response code: %d", httpResponseCode);
        Logger::debug("NETWORK", "Response: %s", response.c_str());
        failedAttempts = 0;
        http.end();
        return true;
    } else {
        Logger::error("NETWORK", "HTTP POST failed, error: %s", http.errorToString(httpResponseCode).c_str());
        failedAttempts++;
        
        if (failedAttempts >= MAX_FAILED_ATTEMPTS) {
            Logger::warn("NETWORK", "Too many failed attempts, buffering data");
            dataBuffer->addData(doc);
            WiFi.disconnect();
            isConnected = false;
            failedAttempts = 0;
        }
        
        http.end();
        return false;
    }
}

bool NetworkManager::isWiFiConnected() const {
    return isConnected;
}

void NetworkManager::connect() {
    Logger::info("NETWORK", "Connecting to WiFi...");
    WiFi.begin(ssid, password);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Logger::info("NETWORK", "Connected to WiFi");
        Logger::info("NETWORK", "IP address: %s", WiFi.localIP().toString().c_str());
        isConnected = true;
    } else {
        Logger::error("NETWORK", "Failed to connect to WiFi");
        isConnected = false;
    }
}

unsigned long NetworkManager::getReconnectDelay() const {
    // Exponential backoff with a maximum delay of 5 minutes
    unsigned long delay = min(1000 * pow(2, failedAttempts), 300000);
    return delay;
}

void NetworkManager::sendBufferedData() {
    if (dataBuffer->isEmpty()) {
        return;
    }
    
    StaticJsonDocument<512> doc;
    while (dataBuffer->getNextData(doc)) {
        if (sendData(doc)) {
            dataBuffer->removeOldest();
        } else {
            break; // Stop if we can't send data
        }
    }
}

} // namespace Network
} // namespace Emopod 