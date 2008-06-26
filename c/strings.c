#include "lib.h"

const char *usage_message = "strings filename...";

enum { min_prefix = 4 };
enum { max_string = 64*1024 };

static int status = 0;

/* TODO: handle object-file types specially (like I'm gonna bother...) */
static void strings (FILE *in, const char *opt_filename) {
  assert (min_prefix <= max_string);
  for (;;) {
    int c = getc (in);
    if (EOF == c)
      goto done;
    if (isprint (c)) {
      char buffer[max_string + 1];
      unsigned i = 0;
      do {
        if (i <= max_string)
          buffer[i++] = c;
        c = getc (in);
        if (EOF == c)
          goto done;
      } while (isprint (c));
      if (min_prefix <= i && ('\0' == c || '\n' == c)) {
        buffer[i] = '\0';
        must_printf ("%s\n", buffer);
        if (max_string < i && 0 == status) {
          fprintf (stderr, "String too long; truncated\n");
          status = 1;
        }
      }
    }
  }
done:
  must_not_ferror (in, opt_filename);
}

void real_main (int argc, char *const argv[]) {
  int i = 1;
  if (i == argc)
    strings (stdin, NULL);
  else
    for (; i < argc; ++i) {
      FILE *in = must_fopen (argv[i], "r");
      strings (in, argv[i]);
      must_fclose (in, argv[i]);
    }
  exit (status);
}
