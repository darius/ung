#include "lib.h"

const char *usage_message = "echo arg...";

void real_main (int argc, char *const argv[]) {
  int i = 1;
  for (; i < argc; ++i)
    must_printf (1 == i ? "%s" : " %s", argv[i]);
  must_printf ("\n");
}
