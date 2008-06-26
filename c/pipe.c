#include "lib.h"

const char *usage_message = "| command... | command...";

/* Run argv1 and argv2 as commands, piping the first's standard output
   into the second's standard input. */
static void run_pipe (char *const argv1[], char *const argv2[]) {
  int fds[2];
  must_pipe (fds);
  pid_t pid1 = must_fork ();
  if (0 == pid1) {
    must_redirect (1, fds[1]);
    must_close (fds[0]);
    must_execvp (argv1);
  }
  else {
    must_redirect (0, fds[0]);
    must_close (fds[1]);
    must_execvp (argv2);
  }
}

/* Return the index of the first element of argv matching "|" with
   at least one token on either side of it (discounting argv[0]).
   Panic if there's none. */
int find_separator (int argc, char *const argv[]) {
  int i;
  for (i = 2; i < argc - 1; ++i)
    if (0 == strcmp ("|", argv[i]))
      return i;
  usage ();
  return 0;  /* To appease the compiler */
}

void real_main (int argc, char *const argv[]) {
  char **mutable_argv = (char **)argv;  /* XXX ugh */
  int separator = find_separator (argc, mutable_argv);
  mutable_argv[separator] = NULL;
  run_pipe (argv + 1, mutable_argv + separator + 1);
}
