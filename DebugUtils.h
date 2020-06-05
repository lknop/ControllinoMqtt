#ifndef DEBUGUTILS_H
#define DEBUGUTILS_H

#include <Arduino.h>

#define TRACE(type, str)    \
    Serial.print(millis()); \
    Serial.print(F(" : ")); \
	Serial.print(F(type)); \
	Serial.print(F(" : ")); \
	Serial.print(F(str)); \
	Serial.print(F(" : ")); \
	Serial.print(__PRETTY_FUNCTION__); \
	Serial.print(F("\n")); \
    Serial.flush();

#define TRACE_DYNAMIC(type, str)    \
    Serial.print(millis()); \
    Serial.print(F(" : ")); \
    Serial.print(F(type)); \
    Serial.print(F(" : ")); \
    Serial.print(str); \
    Serial.print(F(" : ")); \
    Serial.print(__PRETTY_FUNCTION__); \
    Serial.print(F("\n")); \
    Serial.flush();

#ifdef TRACE_LEVEL_OFF
    #ifdef TRACE_LEVEL_ALL
        #undef TRACE_LEVEL_ALL
    #endif
    #ifdef TRACE_LEVEL_DEBUG
        #undef TRACE_LEVEL_DEBUG
    #endif
    #ifdef TRACE_LEVEL_INFO
        #undef TRACE_LEVEL_INFO
    #endif
    #ifdef TRACE_LEVEL_WARN
        #undef TRACE_LEVEL_WARN
    #endif
    #ifdef TRACE_LEVEL_ERROR
        #undef TRACE_LEVEL_ERROR
    #endif
    #ifdef TRACE_LEVEL_FATAL
        #undef TRACE_LEVEL_FATAL
    #endif
#endif

#ifdef TRACE_LEVEL_ALL
    #ifndef TRACE_LEVEL_DEBUG
        #define TRACE_LEVEL_DEBUG
    #endif
#endif  
    
#ifdef TRACE_LEVEL_DEBUG
    #ifndef TRACE_LEVEL_INFO
        #define TRACE_LEVEL_INFO
    #endif
    #define DEBUG_PRINT(str)  TRACE("DEBUG", str)
    #define DEBUG_PRINT_DYNAMIC(str)   TRACE_DYNAMIC("DEBUG", str)
#else
    #define DEBUG_PRINT(str)
    #define DEBUG_PRINT_DYNAMIC(str)
#endif

#ifdef TRACE_LEVEL_INFO
    #ifndef TRACE_LEVEL_WARN
        #define TRACE_LEVEL_WARN
    #endif
    #define INFO_PRINT(str)  TRACE("INFO ", str)
    #define INFO_PRINT_DYNAMIC(str)   TRACE_DYNAMIC("INFO ", str)
#else
    #define INFO_PRINT(str)
    #define INFO_PRINT_DYNAMIC(str)
#endif

#ifdef TRACE_LEVEL_WARN
    #ifndef TRACE_LEVEL_ERROR
        #define TRACE_LEVEL_ERROR
    #endif
    #define WARN_PRINT(str)  TRACE("WARN", str)
    #define WARN_PRINT_DYNAMIC(str)   TRACE_DYNAMIC("WARN", str)
#else
    #define WARN_PRINT(str)
    #define WARN_PRINT_DYNAMIC(str)
#endif

#ifdef TRACE_LEVEL_ERROR
    #ifndef TRACE_LEVEL_FATAL
        #define TRACE_LEVEL_FATAL
    #endif
    #define ERROR_PRINT(str)  TRACE("ERROR", str)
    #define ERROR_PRINT_DYNAMIC(str)   TRACE_DYNAMIC("ERROR", str)
#else
    #define ERROR_PRINT(str)
    #define ERROR_PRINT_DYNAMIC(str)
#endif

#ifdef TRACE_LEVEL_FATAL
    #define FATAL_PRINT(str)  TRACE("FATAL", str)
    #define FATAL_PRINT_DYNAMIC(str)   TRACE_DYNAMIC("FATAL", str)
#else
    #define FATAL_PRINT(str)
    #define FATAL_PRINT_DYNAMIC(str)
#endif



#endif
