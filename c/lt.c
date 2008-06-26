#include "lib.h"
#include "redirector.h"

const char *usage_message = "< filename command...";

void redirect_file (const char *filename) {
  must_redirect (0, must_open (filename, O_RDONLY, 0));
}
