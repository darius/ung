#include <stdio.h>

#include "envlookup.h"

static const char *binding_lookup (const char *binding,
                                   const char *variable) {
  for (;; ++binding, ++variable)
    if (*binding != *variable) {
      if ('=' == *binding && '\0' == *variable)
        return binding + 1;
      return NULL;
    }
    else if ('=' == *binding)
      return NULL;
    else if ('\0' == *binding)
      return NULL;   /* shouldn't happen */
}

const char *env_lookup (char **env, const char *variable) {
  char **p = env;
  for (; NULL != *p; ++p) {
    const char *value = binding_lookup (*p, variable);
    if (NULL != value)
      return value;
  }
  return NULL;
}
