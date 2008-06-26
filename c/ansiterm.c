#include <stdarg.h>

#include "ansiterm.h"

static void ansi (FILE *out, const char *fmt, ...) {
    va_list args;
    va_start (args, fmt);
    fprintf (out, "\x1b[");
    vfprintf (out, fmt, args);
    va_end (args);
}

void aterm_goto (FILE *out, unsigned x, unsigned y) {
    ansi (out, "%u;%uH", x, y);
}

void aterm_home (FILE *out)            { ansi (out, "H"); }
void aterm_clear_screen (FILE *out)    { ansi (out, "2J"); aterm_home (out); }
void aterm_clear_to_eol (FILE *out)    { ansi (out, "K"); }
void aterm_clear_to_bottom (FILE *out) { ansi (out, "J"); }

void aterm_set_attribute (FILE *out, unsigned attr) {
    ansi (out, "%um", attr);
}

void aterm_set_foreground (FILE *out, unsigned color) {
    aterm_set_attribute (out, 30 + color);
}
void aterm_set_background (FILE *out, unsigned color) {
    aterm_set_attribute (out, 40 + color);
}
