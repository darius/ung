#include "lib.h"

const char *usage_message = "rm filename...";

static void must_unlink (const char *filename) {
  if (unlink (filename) < 0)
    sys_panic (filename);
}

void real_main (int argc, char *const argv[]) {
  int i = 1;
  for (; i < argc; ++i)
    must_unlink (argv[i]);
}
