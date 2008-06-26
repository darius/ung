#include "lib.h"
#include "envlookup.h"

extern char **environ;

const char *usage_message = "$ <variable>";

static void print_value (const char *variable) {
  const char *value = env_lookup (environ, variable);
  if (NULL != value)
    printf ("%s", value);
}

void real_main (int argc, char *const argv[]) {
  if (2 != argc)
    usage ();
  print_value (argv[1]);
}
