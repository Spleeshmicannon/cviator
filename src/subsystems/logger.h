/*
 * Note that this file is a recreation of code originally 
 * written by Travis Vroman for the Kohi game engine.
 * */

#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#define CV_TIME_BUFFER 26
#define CV_MESSAGE_BUFFER 1024

typedef enum
{
    CV_LOG_FATAL = 0,
    CV_LOG_ERROR = 1,
    CV_LOG_WARN  = 2,
    CV_LOG_INFO  = 3,
    CV_LOG_DEBUG = 4,
    CV_LOG_TRACE = 5
}CV_logLevel;

const char* CV_log_level_to_string(CV_logLevel level)
{
    static const char* ll_strings[] = {
        "FATAL",
        "ERROR",
        "WARN",
        "INFO",
        "DEBUG",
        "TRACE"
    };

    return ll_strings[level];
}

void CV_log(CV_logLevel level, const char* message, ...)
{
    // create time string
    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    char time_buffer[CV_TIME_BUFFER];
    strftime(time_buffer, CV_TIME_BUFFER, "%Y-%m-%d %H:%M:%S", tm_info);
    
    // process args
    va_list args;
    va_start(args, message);
    char formatted_message[CV_MESSAGE_BUFFER];
    vsnprintf(formatted_message, CV_MESSAGE_BUFFER, message, args);
    va_end(args);

    // print formatted message
    printf("[%s][%s] %s\n", time_buffer, CV_log_level_to_string(level), formatted_message);
}

#define CV_LOG_FATAL(message, ...)  CV_log(CV_LOG_FATAL, message, ##__VA_ARGS__);
#define CV_LOG_ERROR(message, ...)  CV_log(CV_LOG_ERROR, message, ##__VA_ARGS__);
#define CV_LOG_WARN(message, ...)   CV_log(CV_LOG_WARN, message, ##__VA_ARGS__);

#ifdef CV_DEBUG
#define CV_LOG_INFO(message, ...)   CV_log(CV_LOG_INFO, message, ##__VA_ARGS__);
#define CV_LOG_DEBUG(message, ...)  CV_log(CV_LOG_DEBUG, message, ##__VA_ARGS__);
#define CV_LOG_TRACE(message, ...)  CV_log(CV_LOG_TRACE, message, ##__VA_ARGS__);
#else
#define CV_LOG_INFO(message, ...)
#define CV_LOG_DEBUG(message, ...)
#define CV_LOG_TRACE(message, ...)
#endif

#endif // LOGGER_H
