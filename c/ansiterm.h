#ifndef ANSITERM_H
#define ANSITERM_H

#include <stdio.h>

void aterm_goto (FILE *out, unsigned x, unsigned y);
void aterm_home (FILE *out);
void aterm_clear_screen (FILE *out);
void aterm_clear_to_eol (FILE *out);
void aterm_clear_to_bottom (FILE *out);

enum {
    aterm_normal    = 0,
    aterm_bright,
    aterm_dim,                        // N.B. dim doesn't work for me.
    aterm_underline = 4,
    aterm_blink,
    aterm_reverse   = 7,
    aterm_hidden
};
void aterm_set_attribute (FILE *out, unsigned attribute);

enum {
    aterm_black = 0,
    aterm_red,
    aterm_green,
    aterm_yellow,
    aterm_blue,
    aterm_magenta,
    aterm_cyan,
    aterm_white
};
void aterm_set_foreground (FILE *out, unsigned color);
void aterm_set_background (FILE *out, unsigned color);

#endif
