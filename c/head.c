#include "lib.h"

const char *usage_message = "head -<nlines>";

static void copy_lines (unsigned long n) {
  for (; 0 < n; --n) {
    for (;;) {
      int c = getc (stdin);
      if (EOF == c) {
        if (ferror (stdin))
          sys_panic (NULL);
        return;
      }
      must_putc (c, stdout);
      if ('\n' == c)
        break;
    }
  }
}

void real_main (int argc, char *const argv[]) {
  if (argc != 2)
    usage ();         /* TODO: allow filename argument */
  if (argv[1][0] != '-')
    usage ();
  copy_lines (must_strtoul (argv[1] + 1, 10));
}
