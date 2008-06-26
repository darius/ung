#include "lib.h"

const char *usage_message = "uniq";

enum { capacity = 64*1024 };

typedef struct Line Line;
struct Line {
  size_t length;
  char   chars[capacity];
};

static void append (Line *line, char c) {
  if (line->length == capacity)
    panic ("Line too long");
  line->chars[line->length++] = c;
}

static void line_read (Line *line) {
  line->length = 0;
  for (;;) {
    int c = getc (stdin);
    if (EOF == c) {
      must_not_ferror (stdin, NULL);
      return;
    }
    append (line, c);
    if ('\n' == c)
      return;
  }
}

static void line_write (const Line *line) {
  size_t i;
  for (i = 0; i < line->length; ++i)
    must_putc (line->chars[i], stdout);
}

static int lines_match (const Line *line1, const Line *line2) {
  return (line1->length == line2->length
          && 0 == memcmp (line1->chars, line2->chars, line1->length));
}

static void uniq (void) {
  Line line1, line2;
  Line *prev = &line1;
  Line *next = &line2;

  line_read (prev);
  line_write (prev);
  while (!feof (stdin)) {
    line_read (next);
    if (!lines_match (prev, next)) {
      Line *tmp = prev;
      prev = next;
      next = tmp;
      line_write (prev);
    }
  }
}

void real_main (int argc, char *const argv[]) {
  (void) argv;
  if (1 == argc)
    uniq ();
  else
    usage ();
}
