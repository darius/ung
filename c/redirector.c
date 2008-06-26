#include "lib.h"
#include "redirector.h"

void real_main (int argc, char *const argv[]) {
  if (argc < 3)
    usage ();  /* or maybe we should default to 'cat' behavior */
  redirect_file (argv[1]);
  must_execvp (argv + 2);
}
