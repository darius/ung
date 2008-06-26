#include "lib.h"

const char *program_name;

int panic_code = 1;

void usage (void) {
  fprintf (stderr, "usage: %s\n", usage_message);
  exit (panic_code);
}

void complain2 (const char *opt_plaint1, const char *plaint2) {
  fprintf (stderr, "%s: ", program_name);
  if (opt_plaint1)
    fprintf (stderr, "%s: ", opt_plaint1);
  fprintf (stderr, "%s\n", plaint2);
}

void complain (const char *plaint) {
  complain2 (NULL, plaint);
}

void panic (const char *plaint) {
  complain (plaint);
  exit (panic_code);
}

void sys_complain (const char *opt_plaint) {
  complain2 (opt_plaint, strerror (errno));
}

void sys_panic (const char *opt_plaint) {
  sys_complain (opt_plaint);
  exit (panic_code);
}

pid_t must_fork (void) {
  pid_t pid = fork ();
  if (pid < 0)
    sys_panic ("fork");
  return pid;
}

int must_open (const char *filename, int flags, mode_t mode) {
  int fd = open (filename, flags, mode);
  if (fd < 0)
    sys_panic (filename);
  return fd;
}

void must_close (int fd) {
  if (close (fd) < 0)
    sys_panic ("close");
}

void must_redirect (int new_fd, int old_fd) {
  // Pre: new_fd != old_fd
  /* XXX use dup2 i guess */
  must_close (new_fd);
  if (dup (old_fd) < 0)
    sys_panic ("dup");
  must_close (old_fd);
}

void must_pipe (int fds[]) {
  if (pipe (fds) < 0)
    sys_panic ("pipe");
}

void must_execvp (char *const argv[]) {
  execvp (argv[0], argv);
  panic_code = 127;
  sys_panic (argv[0]);
}

FILE *must_fopen (const char *filename, const char *mode) {
  FILE *f = fopen (filename, mode);
  if (!f) sys_panic (filename);
  return f;
}

void must_fclose (FILE *f, const char *opt_filename) {
  if (0 != fclose (f))
    sys_panic (opt_filename);
}

/* N.B. This must be called immediately after the potentially failing
   I/O operation while errno would still be valid. */
void must_not_ferror (FILE *f, const char *opt_filename) {
  if (ferror (f))
    sys_panic (opt_filename);
}

void *must_malloc (size_t size) {
  void *result = malloc (size);
  if (!result && 0 < size)
    sys_panic ("malloc");
  return result;
}

void *must_realloc (void *ptr, size_t size) {
  void *result = realloc (ptr, size);
  if (!result && 0 < size)
    sys_panic ("realloc");
  return result;
}

unsigned long must_strtoul (const char *s, unsigned base) {
  char *end;
  errno = 0;
  unsigned long result = strtoul (s, &end, base);
  if (0 != errno)
    sys_panic (s);
  if (end == s)
    panic ("Missing number argument");
  if ('\0' != *end)
    panic ("Number parsed with stuff left over");
  return result;
}

void must_copy (FILE *out, FILE *in, const char *in_filename) {
  int c;
  while (EOF != (c = getc (in)))
    must_putc (c, out);
  must_not_ferror (in, in_filename);
}

size_t must_grow (size_t size) {
  size_t result = (0 == size ? 1 : 2 * size);
  if (result <= size)
    panic ("Overflow");
  return result;
}

int main (int argc, char *const argv[]) {
  program_name = argv[0];
  real_main (argc, argv);
  return 0;
}
