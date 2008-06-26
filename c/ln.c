#include "lib.h"

const char *usage_message = "ln from-file to-file";

static void must_link (const char *from_filename, const char *to_filename) {
  if (link (from_filename, to_filename) < 0)
    sys_panic (NULL);
  /* TODO: improve on NULL */
}

void real_main (int argc, char *const argv[]) {
  int i = 1;
  if (argc - i != 2)
    usage ();
  must_link (argv[i], argv[i+1]);
}
