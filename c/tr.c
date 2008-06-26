#include "lib.h"

const char *usage_message = "tr from-charset to-charset";

static char translit (const char *from_chars, const char *to_chars, char c) {
  const char *s = strchr (from_chars, c);
  if (!s)
    return c;
  else {
    size_t L = strlen (to_chars);  /* TODO: precompute */
    assert (0 < L);
    if ((size_t)(s - from_chars) < L)
      return to_chars[s - from_chars];
    else
      return to_chars[L - 1];
  }
}

static void tr (const char *from_chars, const char *to_chars) {
  int c;
  while (EOF != (c = getc (stdin))) {
    char translated = translit (from_chars, to_chars, c);
    must_putc (translated, stdout);
  }
  must_not_ferror (stdin, NULL);
}

void real_main (int argc, char *const argv[]) {
  if (3 == argc)
    tr (argv[1], argv[2]);
  else
    usage ();
}
