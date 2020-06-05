#ifndef DEBUGUTILS_H
#define DEBUGUTILS_H

#include <Arduino.h>
#include <ArduinoSTL.h>




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

#define TRACE_PARAM(type, str, param)    \
    Serial.print(millis()); \
    Serial.print(F(" : ")); \
	Serial.print(F(type)); \
	Serial.print(F(" : ")); \
	Serial.print(F(str)); \
	Serial.print(F(" : ")); \
	Serial.print(param); \
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
    #define DEBUG_PRINT_PARAM(str, param)   TRACE_PARAM("DEBUG", str, param)
#else
    #define DEBUG_PRINT(str)
    #define DEBUG_PRINT_PARAM(str, param)
#endif

#ifdef TRACE_LEVEL_INFO
    #ifndef TRACE_LEVEL_WARN
        #define TRACE_LEVEL_WARN
    #endif
    #define INFO_PRINT(str)  TRACE("INFO ", str)
    #define INFO_PRINT_PARAM(str, param)   TRACE_PARAM("INFO ", str, param)
#else
    #define INFO_PRINT(str)
    #define INFO_PRINT_PARAM(str, param)
#endif

#ifdef TRACE_LEVEL_WARN
    #ifndef TRACE_LEVEL_ERROR
        #define TRACE_LEVEL_ERROR
    #endif
    #define WARN_PRINT(str)  TRACE("WARN", str)
    #define WARN_PRINT_PARAM(str, param)   TRACE_PARAM("WARN", str, param)
#else
    #define WARN_PRINT(str)
    #define WARN_PRINT_PARAM(str, param)
#endif

#ifdef TRACE_LEVEL_ERROR
    #ifndef TRACE_LEVEL_FATAL
        #define TRACE_LEVEL_FATAL
    #endif
    #define ERROR_PRINT(str)  TRACE("ERROR", str)
    #define ERROR_PRINT_PARAM(str, param)   TRACE_PARAM("ERROR", str, param)
#else
    #define ERROR_PRINT(str)
    #define ERROR_PRINT_PARAM(str, param)
#endif

#ifdef TRACE_LEVEL_FATAL
    #define FATAL_PRINT(str)  TRACE("FATAL", str)
    #define FATAL_PRINT_PARAM(str, param)   TRACE_PARAM("FATAL", str, param)
#else
    #define FATAL_PRINT(str)
    #define FATAL_PRINT_PARAM(str, param)
#endif



#endif
