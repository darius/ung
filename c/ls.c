#include "lib.h"

//#include <sys/dirent.h>
#include <dirent.h>

const char *usage_message = "ls";

static void ls_entry (const struct dirent *entry) {
  must_printf ("%s\n", entry->d_name);
}

static void ls_entries (const struct dirent *entries, int nbytes) {
  while (0 < nbytes) {
    if (entries->d_fileno != 0)
      ls_entry(entries);
    nbytes -= entries->d_reclen;
    entries = (const struct dirent *)
               ((const char *)entries + entries->d_reclen);
  }
}

static void ls_dir (const char *filename) {
  int fd = open (filename, O_RDONLY, 0);
  if (fd < 0)
    sys_panic (filename);
  { // XXX use opendir(3) and readdir(3) instead for POSIXicity
    struct dirent entries[16384];
    long base;
    int nbytes = getdirentries (fd, entries, sizeof entries, &base);
    if (nbytes < 0)
      sys_panic (filename);
    /* XXX loop until there are no more */
    ls_entries (entries, nbytes);
  }
  must_close (fd);
}

void real_main (int argc, char *const argv[]) {
  (void) argv;
  if (1 != argc)
    usage ();
  ls_dir (".");
}
