#ifndef APM_EXT_LOGGING_H
#define APM_EXT_LOGGING_H

#include <cstdio>
#include <ctime>
#include <cstdarg>

inline void log_with_time(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    std::fprintf(stderr, "Time: %lu ", (unsigned long)time(NULL));
    std::vfprintf(stderr, fmt, args);
    std::fprintf(stderr, "\n");
    va_end(args);
}

#endif // APM_EXT_LOGGING_H

