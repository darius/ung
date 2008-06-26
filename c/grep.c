#include "lib.h"
#include "regexpoid.h"

const char *usage_message = "grep [-inv] pattern filename...";

enum { line_capacity = 64 * 1024 };

static int showing_filenames    = 0;
static int showing_line_numbers = 0;
static int matching_case        = 1;
static int matching_positively  = 1;

static void fold_case (char *dest, const char *src) {
  for (; *src; ++dest, ++src)
    *dest = tolower (*src);
}

/* Remove the trailing newline from line. */
static void trim_newline (char *line, const char *opt_filename) {
  char *nl = strchr (line, '\n');
  if (nl)
    *nl = '\0';
  else {
    complain2 (opt_filename, "line too long");
    exit (panic_code);
  }
}

/* Return true iff regexp matches line (according to the case switch). */
static int matches (const char *regexp, const char *line) {
  if (matching_case)
    return regexpoid_matches (regexp, line);
  else {
    char folded[line_capacity];
    fold_case (folded, line);
    return regexpoid_matches (regexp, folded);
  }
}

static void show (const char *opt_filename,
                  unsigned line_num,
                  const char *line) {
  if (showing_filenames)    must_printf ("%s:", opt_filename);
  if (showing_line_numbers) must_printf ("%u:", line_num);
  must_printf ("%s\n", line);
}

/* Show any matching lines from 'in'. Return true on success. */
static int grep (const char *regexp, FILE *in, const char *opt_filename) {
  int found = 0;
  unsigned line_num = 0;
  char line[line_capacity];

  char folded_regexp[line_capacity];
  if (!matching_case) {
    if (line_capacity <= strlen (regexp))
      panic ("regexp too long");
    fold_case (folded_regexp, regexp);
    regexp = folded_regexp;
  }

  while (fgets (line, sizeof line, in)) {
    trim_newline (line, opt_filename);
    ++line_num;
    if (matching_positively == matches (regexp, line)) {
      found = 1;
      show (opt_filename, line_num, line);
    }
  }
  must_not_ferror (in, opt_filename ? opt_filename : "<stdin>");
  return found;
}

static void parse_options (const char *options) {
  for (; *options; ++options)
    switch (*options) {
      case 'i': matching_case = 0;        break;
      case 'n': showing_line_numbers = 1; break;
      case 'v': matching_positively = 0;  break;
      default: usage ();
    }
}

void real_main (int argc, char *const argv[]) {
  int found = 0;
  int i = 1;
  panic_code = 2;
  for (; i < argc && argv[i][0] == '-'; ++i)
    parse_options (argv[i] + 1);
  if (i == argc)
    usage ();
  else if (i + 1 == argc)
    found = grep (argv[i], stdin, NULL);
  else {
    const char *regexp = argv[i++];
    showing_filenames = (1 < argc - i);
    for (; i < argc; ++i) {
      FILE *in = must_fopen (argv[i], "r");
      found |= grep (regexp, in, argv[i]);
      must_fclose (in, argv[i]);
    }
  }
  exit (!found);
}
