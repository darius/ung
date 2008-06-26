#include "lib.h"

const char *usage_message = "chmod permissions filename";

static void must_chmod (const char *filename, mode_t mode) {
  if (chmod (filename, mode) < 0)
    sys_panic (filename);
}

static mode_t parse_mode (const char *mode) {
  return must_strtoul (mode, 8);
}

void real_main (int argc, char *const argv[]) {
  int i = 1;
  if (argc - i != 2)
    usage ();
  must_chmod (argv[i+1], parse_mode (argv[i]));
}
