#include "lib.h"

const char *usage_message = "sort";

static size_t capacity = 0;
static size_t size     = 0;
static char*  buffer   = NULL;
static size_t n_lines  = 0;

/* Append ::c to the buffer. */
static void append (char c) {
  if (size == capacity) {
    capacity = must_grow (capacity);
    buffer = must_realloc (buffer, capacity);
  }
  if ('\n' == c) {
    buffer[size++] = '\0';
    ++n_lines;
  }
  else
    buffer[size++] = c;
}

/* Append the contents of ::in to the buffer. */
static void snarf (FILE *in, const char *opt_filename) {
  int c;
  while (EOF != (c = getc (in)))
    append (c);
  must_not_ferror (in, opt_filename);
  if (0 < size && '\0' != buffer[size - 1])
    append ('\n');
}

typedef const char *Line;
static Line *lines;

/* Fill the lines array from the buffer. */
static void prepare (void) {
  lines = must_malloc (n_lines * sizeof lines[0]);
  {
    size_t L, c;
    for (L = c = 0; L < n_lines; ++L) {
      lines[L] = buffer + c;
      c += strlen (buffer + c) + 1;
    }
  }
}

static int compare_lines (const void *a1, const void *a2) {
  return strcmp (*(const Line *)a1, *(const Line *)a2);
}

/* Sort the lines array. */
static void sort (void) {
  qsort (lines, n_lines, sizeof lines[0], compare_lines);
}

/* Write the lines array to stdout. */
static void output (void) {
  size_t L;
  for (L = 0; L < n_lines; ++L)
    must_printf ("%s\n", lines[L]);
}

void real_main (int argc, char *const argv[]) {
  (void) argv;
  if (1 == argc)
    snarf (stdin, NULL);
  else
    usage ();
  prepare ();
  sort ();
  output ();
}
