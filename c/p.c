#include "ansiterm.h"
#include "lib.h"

const char *usage_message = "p [filename...]";

enum {   // XXX get these from the actual terminal somehow
    screen_cols = 80,
    screen_rows = 44
};

static void quit (void) {
    system ("stty -f /dev/tty sane");
    exit (0);
}

static unsigned row = 0;
static unsigned col = 0;

static void put_c (int c, unsigned delta) {
    putc (c, stdout);
    col += delta;
    if (screen_cols <= col)
        ++row, col = 0;
}

static void pause_for_page (const char *prompt, int tty) {
    aterm_set_attribute (stdout, aterm_reverse);
    printf ("%s", prompt);
    aterm_set_attribute (stdout, aterm_normal);
    fflush (stdout);
    char key;
    read (tty, &key, 1);  // XXX check for error
    putc ('\r', stdout);
    aterm_clear_to_eol (stdout);
    if ('q' == key)
        quit ();
}

static void p (FILE *in, const char *opt_filename, int tty) {
    for (;;) {
        int c = getc (in);
        if (EOF == c) break;
        if (screen_rows <= row + 1) {
            const char *prompt = opt_filename ? opt_filename : "--more--";
            pause_for_page (prompt, tty);
            row = col = 0;
        }
        if (isprint (c))
            put_c (c, 1);
        else if ('\t' == c)
            put_c (c, 8 - col % 8);
        else if ('\n' == c) {
            printf ("\r\n");
            ++row, col = 0;
        }
        else {
            aterm_set_attribute (stdout, aterm_reverse);
            put_c ('?', 1);  // XXX for now
            aterm_set_attribute (stdout, aterm_normal);
        }
    }
    must_not_ferror (in, opt_filename);
}

void real_main (int argc, char *const argv[]) {
    system ("stty -f /dev/tty raw");
    int tty = must_open ("/dev/tty", O_RDONLY, 0);
    int i = 1;
    if (i == argc)
        p (stdin, NULL, tty);
    else
        for (i = 1; i < argc; ++i) {
            FILE *f = must_fopen (argv[i], "r");
            p (f, argv[i], tty);
            must_fclose (f, argv[i]);
        }
    must_close (tty);
    system ("stty -f /dev/tty sane");
    // XXX the previous line should in effect be in a 'finally' block
}
