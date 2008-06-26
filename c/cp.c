#include "lib.h"

const char *usage_message = "cp source-file destination-file";

static void copy_named_file (FILE *out, const char *in_filename) {
  FILE *in = must_fopen (in_filename, "r");
  must_copy (out, in, in_filename);
  must_fclose (in, in_filename);
}

void real_main (int argc, char *const argv[]) {
  int i = 1;
  /* TODO: -i and -r options, at least */

  if (argc - i == 2) {
    const char *dest_name = argv[argc - 1];
    FILE *dest = must_fopen (dest_name, "w");
    /* XXX check if src and dest are the same */
    copy_named_file (dest, argv[i]);
    must_fclose (dest, dest_name);
  }
  else         /* TODO: cp to a directory */
    usage ();
}
