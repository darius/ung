#include "lib.h"

const char *usage_message = "pwd";

static void pwd (void) {
  char *filename = getcwd (NULL, 0);
  if (!filename)
    sys_panic (NULL);
  must_printf ("%s\n", filename);
  free (filename);
}

void real_main (int argc, char *const argv[]) {
  (void) argv;
  if (1 != argc)
    usage ();
  pwd ();
}
