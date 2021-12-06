#include "util.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

void fatal_impl(
  const char *function,
  const char *filename,
  int line_number, 
  const char *format, ...
) {
  // PORT: none of these apis are available on esp32
  va_list args;
  va_start(args, format);

  fprintf(stderr,
    "FATAL ERROR\n"
    "%s() from %s in line %d\n\n",
    function, filename, line_number
  );
  vfprintf(stderr, format, args);
  fprintf(stderr, "\n");

  va_end(args);
  exit(1);
}

void assert_fatal_impl(
  bool condition, 
  const char *function,
  const char *filename,
  int line_number,
  const char *assertion,
  const char *format, ...
) {
  if (condition) return;

  // PORT: none of these apis are available on esp32
  va_list args;
  va_start(args, format);

  fprintf(stderr,
    "FAILED ASSERTION\n"
    "%s() from %s in line %d\n    %s\n",
    function, filename, line_number, assertion
  );

  if (format != NULL) {
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
  }

  va_end(args);
  exit(1);
}
