/* Headers used by more or less everyone. */

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


/* The library's imports. */

extern const char *usage_message;
extern void real_main (int argc, char *const argv[]);


/* The library's exports. */

extern const char *program_name;
extern int panic_code;

extern void usage (void);

extern void complain2 (const char *opt_plaint1, const char *plaint2);
extern void complain (const char *plaint);
extern void panic    (const char *plaint);
extern void sys_complain (const char *opt_plaint);
extern void sys_panic    (const char *opt_plaint);

pid_t must_fork (void);
int   must_open (const char *filename, int flags, mode_t mode);
void  must_close (int fd);
void  must_redirect (int new_fd, int old_fd);
void  must_pipe (int fds[]);
void  must_execvp (char *const argv[]);

extern FILE *must_fopen (const char *filename, const char *mode);
extern void must_fclose (FILE *f, const char *opt_filename);
extern void must_not_ferror (FILE *f, const char *opt_filename);

extern void *must_malloc (size_t size);
extern void *must_realloc (void *ptr, size_t size);

extern unsigned long must_strtoul (const char *s, unsigned base);

/* XXX fill in must_putc and must_printf */
#define must_putc   putc
#define must_printf printf

extern void must_copy (FILE *out, FILE *in, const char *in_filename);

extern size_t must_grow (size_t size);

extern int main (int argc, char *const argv[]);
