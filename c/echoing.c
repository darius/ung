#include <stdio.h>

/* This is just to test my shell's argument-parsing */

int main (int argc, char **argv) {
  int i = 1;
  for (; i < argc; ++i)
    printf ("[%s]\n", argv[i]);
  return 0;
}
