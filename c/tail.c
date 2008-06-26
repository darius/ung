#include "lib.h"

const char *usage_message = "tail +<nlines>";

static void skip_lines (unsigned long n) {
  for (; 0 < n; --n)
    for (;;) {
      int c = getc (stdin);
      if (EOF == c) {
        must_not_ferror (stdin, NULL);
        return;
      }
      if ('\n' == c)
        break;
    }
}

void real_main (int argc, char *const argv[]) {
  if (argc != 2)
    usage ();         /* TODO: allow filename argument */
  if (argv[1][0] != '+') /* TODO: other options */
    usage ();
  {
    unsigned long n = must_strtoul (argv[1] + 1, 10);
    if (0 == n)
      panic ("Line numbers start at 1");
    skip_lines (n - 1);
    must_copy (stdout, stdin, NULL);
  }
}
