#ifndef DATA_BUFFER_H
#define DATA_BUFFER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "utils/Logger.h"

namespace Emopod {
namespace Utils {

class DataBuffer {
private:
    static const int MAX_ENTRIES = 50;
    static const int JSON_SIZE = 512;
    
    struct BufferEntry {
        char jsonData[JSON_SIZE];
        unsigned long timestamp;
        bool isUsed;
    };
    
    BufferEntry entries[MAX_ENTRIES];
    int head;
    int tail;
    int count;
    
public:
    DataBuffer() : head(0), tail(0), count(0) {
        for (int i = 0; i < MAX_ENTRIES; i++) {
            entries[i].isUsed = false;
        }
    }
    
    bool addData(const JsonDocument& doc) {
        if (count >= MAX_ENTRIES) {
            Logger::warn("BUFFER", "Buffer full, discarding oldest entry");
            removeOldest();
        }
        
        entries[tail].timestamp = millis();
        entries[tail].isUsed = true;
        
        if (serializeJson(doc, entries[tail].jsonData, JSON_SIZE) == 0) {
            Logger::error("BUFFER", "Failed to serialize JSON");
            return false;
        }
        
        tail = (tail + 1) % MAX_ENTRIES;
        count++;
        return true;
    }
    
    bool getNextData(JsonDocument& doc) {
        if (count == 0) {
            return false;
        }
        
        DeserializationError error = deserializeJson(doc, entries[head].jsonData);
        if (error) {
            Logger::error("BUFFER", "Failed to deserialize JSON: %s", error.c_str());
            removeOldest();
            return false;
        }
        
        return true;
    }
    
    void removeOldest() {
        if (count > 0) {
            entries[head].isUsed = false;
            head = (head + 1) % MAX_ENTRIES;
            count--;
        }
    }
    
    int getCount() const {
        return count;
    }
    
    bool isFull() const {
        return count >= MAX_ENTRIES;
    }
    
    bool isEmpty() const {
        return count == 0;
    }
    
    void clear() {
        head = 0;
        tail = 0;
        count = 0;
        for (int i = 0; i < MAX_ENTRIES; i++) {
            entries[i].isUsed = false;
        }
    }
};

} // namespace Utils
} // namespace Emopod

#endif 