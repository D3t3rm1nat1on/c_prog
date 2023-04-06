#ifndef PRINTS_H
#define PRINTS_H

#include <string.h>

void print_args(int argc, char* argv[]);
void print_id();
void prints_id();

// int info(const char* fname, int lineno, const char* fxname, ...);
// int debug(const char* fname, int lineno, const char* fxname, ...);
// int error(const char* fname, int lineno, const char* fxname, ...);

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define log_info(...)                                                                               \
    fprintf(stderr, "info: ");                                                                      \
    fprintf(stderr, __VA_ARGS__);

#define log_error(...)                                                                              \
    fprintf(stderr, "error: %s:%d: ", __FILENAME__, __LINE__);                                      \
    fprintf(stderr, __VA_ARGS__);

#define log_debug(...)                                                                              \
    fprintf(stderr, "debug: %s:%s:%d: ", __FILE__, __func__, __LINE__);                             \
    fprintf(stderr, __VA_ARGS__);

#endif
