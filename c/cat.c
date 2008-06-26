#include "lib.h"

const char *usage_message = "can't happen";

void real_main (int argc, char *const argv[]) {
  int i = 1;
  if (i == argc)
    must_copy (stdout, stdin, NULL);
  else
    for (; i < argc; ++i) {
      FILE *in = must_fopen (argv[i], "r");
      must_copy (stdout, in, argv[i]);
      must_fclose (in, argv[i]);
    }
}
