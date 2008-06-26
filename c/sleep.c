#include "lib.h"

const char *usage_message = "sleep <seconds>";

static unsigned parse_unsigned (const char *s) {
  unsigned long n = must_strtoul (s, 10);
  if (n != (unsigned) n)
    panic ("Numeric overflow");
  return (unsigned) n;
}

void real_main (int argc, char *const argv[]) {
  if (2 != argc)
    usage ();
  sleep (parse_unsigned (argv[1]));
}
