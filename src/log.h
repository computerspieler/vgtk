#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>

#define ERROR(...)  fprintf(stderr, "[ERR] " __VA_ARGS__);
#define WARN(...)   fprintf(stderr, "[WAR] " __VA_ARGS__);
#define LOG(...)    fprintf(stderr, "[LOG] " __VA_ARGS__);

#endif

