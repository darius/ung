#include "lib.h"

extern char **environ;

const char *usage_message = "can't happen";

enum {          max_bindings = 1024 };
static char *   env[max_bindings];
static unsigned size = 0;

static void env_append (char *binding) {
  if (size == max_bindings)
    panic ("Too many environment variables");
  env[size++] = binding;
}

static int is_assignment (const char *s) {
  return NULL != strchr (s, '=');
}

static int same_key (const char *binding1, const char *binding2) {
  for (;; ++binding1, ++binding2)
    if (*binding1 != *binding2)
      return 0;
    else if ('=' == *binding1)
      return 1;
    else if ('\0' == *binding1)
      return 0;   /* shouldn't happen */
}

static void env_parse_assignment (char *assignment) {
  unsigned i;
  for (i = 0; i < size; ++i)
    if (same_key (env[i], assignment)) {
      env[i] = assignment;
      return;
    }
  env_append (assignment);
}

static void env_import (void) {
  char **p = environ;
  for (; NULL != *p; ++p)
    env_append (*p);
}

static void env_export (void) {
  env_append (NULL);
  environ = env;
}

static void env_print (void) {
  unsigned i;
  for (i = 0; i < size; ++i)
    must_printf ("%s\n", env[i]);
}

void real_main (int argc, char *const argv[]) {
  int i;
  /* TODO: handle -i */
  env_import ();
  for (i = 1; i < argc && is_assignment (argv[i]); ++i)
    env_parse_assignment (argv[i]);
  if (i == argc)
    env_print ();
  else {
    env_export ();
    must_execvp (argv + i);
  }
}
