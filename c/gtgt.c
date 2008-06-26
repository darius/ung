#include "lib.h"
#include "redirector.h"

const char *usage_message = ">> filename command...";

void redirect_file (const char *filename) {
  must_redirect (1, must_open (filename,
                               O_WRONLY | O_APPEND,
                               0666));             /* I guess, again */
}
