/* TODO can't we write this in sh? */

#include "lib.h"

const char *usage_message = "clear";

void real_main (int argc, char *const argv[]) {
  (void) argv;
  if (argc != 1)
    usage ();
  /* XXX terminal-specific */
  must_printf ("\x1b\x5b\x48\x1b\x5b\x32\x4a");
  must_putc ('\0', stdout);
}
