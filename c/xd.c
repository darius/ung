#include "lib.h"

const char *usage_message = "xd filename...";

static unsigned n = 0;

enum { line_length = 16 };
static char line[line_length + 1];

static void print_n (void) {
  must_printf ("%08x", n);
}

static void flush_line (void) {
  unsigned i;
  unsigned limit = (0 == n % line_length ? line_length : n % line_length);
  for (i = 0; i < limit; ++i) {
    if (0 == i % 8)
      must_printf (" ");
    must_printf (" %02x", 0xFF & line[i]);
  }
  must_printf ("\n");
  print_n ();
}

static void accept (char c) {
  line[n % line_length] = c;
  ++n;
  if (0 == n % line_length)
    flush_line ();
}

static void finish (void) {
  if (0 < n % line_length)
    flush_line ();
  must_printf ("\n");
}

static void xd (FILE *in, const char *opt_filename) {
  int c;
  while (EOF != (c = getc (in)))
    accept (c);
  must_not_ferror (in, opt_filename);
}

void real_main (int argc, char *const argv[]) {
  int i = 1;
  print_n ();
  if (i == argc)
    xd (stdin, NULL);
  else
    for (; i < argc; ++i) {
      FILE *in = must_fopen (argv[i], "r");
      xd (in, argv[i]);
      must_fclose (in, argv[i]);
    }
  finish ();
}
