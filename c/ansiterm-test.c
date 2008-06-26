#include <stdio.h>
#include <stdlib.h>

#include "ansiterm.h"

int main (void) {
    aterm_clear_screen (stdout);

    aterm_goto (stdout, 5, 10);             // N.B. 1-based
    aterm_set_attribute (stdout, aterm_normal);
    printf ("hello world\n");

    aterm_goto (stdout, 1, 1);             // N.B. 1-based
    aterm_set_attribute (stdout, aterm_bright);
    aterm_set_attribute (stdout, aterm_underline);
    printf ("foo");
    aterm_clear_to_eol (stdout);
    printf ("\n");

    aterm_set_attribute (stdout, aterm_normal);

    system ("stty raw");
    int c = getchar ();
    system ("stty sane");

    return 0;
}
