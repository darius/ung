#include "lib.h"

const char *usage_message = "lc filename...";

static void lc (FILE *in, const char *opt_filename) {
  unsigned n_lines = 0;

  int c;
  while (EOF != (c = getc (in)))
    if ('\n' == c)
      ++n_lines;
  must_not_ferror (in, opt_filename);

  must_printf ("%8u", n_lines);
  if (opt_filename)
    must_printf (" %s", opt_filename);
  must_printf ("\n");
}

void real_main (int argc, char *const argv[]) {
  int i = 1;
  if (i == argc)
    lc (stdin, NULL);
  else
    for (; i < argc; ++i) {
      FILE *in = must_fopen (argv[i], "r");
      lc (in, argv[i]);
      must_fclose (in, argv[i]);
    }
}
