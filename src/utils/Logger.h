#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

namespace Emopod {
namespace Utils {

class Logger {
private:
    static const char* getLevelString(int level) {
        switch (level) {
            case 0: return "[DEBUG]";
            case 1: return "[INFO]";
            case 2: return "[WARN]";
            case 3: return "[ERROR]";
            default: return "[UNKNOWN]";
        }
    }

public:
    static void log(int level, const char* module, const char* format, ...) {
        #ifdef DEBUG
        char buffer[256];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        
        Serial.printf("%s %s %s\n", getLevelString(level), module, buffer);
        #endif
    }
    
    static void debug(const char* module, const char* format, ...) {
        #ifdef DEBUG
        char buffer[256];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        
        Serial.printf("%s %s %s\n", getLevelString(0), module, buffer);
        #endif
    }
    
    static void info(const char* module, const char* format, ...) {
        char buffer[256];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        
        Serial.printf("%s %s %s\n", getLevelString(1), module, buffer);
    }
    
    static void warn(const char* module, const char* format, ...) {
        char buffer[256];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        
        Serial.printf("%s %s %s\n", getLevelString(2), module, buffer);
    }
    
    static void error(const char* module, const char* format, ...) {
        char buffer[256];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        
        Serial.printf("%s %s %s\n", getLevelString(3), module, buffer);
    }
};

} // namespace Utils
} // namespace Emopod

#endif 