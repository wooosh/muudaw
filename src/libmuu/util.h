#pragma once

#include <stdbool.h>
#include <string.h>
#include <errno.h>

#define MSG_ERRNO "%s", strerror(errno)

#define fatal(...)\
  fatal_impl(__func__, __FILE__, __LINE__, __VA_ARGS__)

void fatal_impl(
  const char *function,
  const char *filename,
  int line_number, 
  const char *format,
  ...
);

#ifndef NDEBUG 
#define assert_fatal(cond, ...)\
  assert_fatal_impl((cond), __func__, __FILE__, __LINE__, #cond,\
    __VA_ARGS__)
#else
  #define assert_fatal(cond, ...)
#endif

void assert_fatal_impl(
  bool condition, 
  const char *function,
  const char *filename,
  int line_number,
  const char *assertion,
  const char *format,
  ...
);
