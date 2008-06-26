#include <sys/wait.h>

#include "envlookup.h"
#include "lib.h"

const char *usage_message = "sh";

extern char **environ;

static void trim_newline (char *s) {
  size_t L = strlen (s);
  if (0 < L && s[L-1] == '\n')
    s[L-1] = '\0';
  else
    panic ("Line too long");
}

// The strings pool

enum { pool_capacity = 64*1024 };
static char pool[pool_capacity];
static unsigned pool_ptr = 0;

static void enpool (char c) {
  if (pool_capacity <= pool_ptr)
    panic ("Out of string space");
  pool[pool_ptr++] = c;
}

enum { strings_capacity = 4*1024 };
static char *strings[strings_capacity];
static unsigned strings_ptr = 0;

static void strings_append (char *s) {
  if (strings_capacity <= strings_ptr)
    panic ("Too many strings");
  strings[strings_ptr++] = s;
}

static void reclaim_strings (void) {
  pool_ptr = 0;
  strings_ptr = 0;
}

static void strings_dump (void) {
  unsigned i;
  for (i = 0; i < strings_ptr; ++i)
    printf ("%u: [%s]\n", i , strings[i]);
}

// Delete all the strings between strings_start and output_start,
// shifting whatever's left above back down to strings_start.
static void collapse (unsigned pool_start, unsigned strings_start,
                      unsigned output_start) {
  printf ("Before collapse %u %u (%u):\n",
          strings_start, output_start, strings_ptr);
  strings_dump ();

  unsigned output_end = strings_ptr;
  if (0) pool_ptr = pool_start;
  strings_ptr = strings_start;
  
  unsigned i;
  for (i = output_start; i < output_end; ++i) {
    if (0) { // XXX original code
      unsigned size = strlen (strings[i]) + 1;
      strings[strings_ptr++] = pool + pool_ptr;
      memmove (pool + pool_ptr, strings[i], size);
      pool_ptr += size;
    }
    else {
      strings[strings_ptr++] = strings[i];
    }
  }
  printf ("After collapse %u %u:\n", strings_start, output_start);
  strings_dump ();
}

// Running parsed commands

static void wait_for (pid_t pid) {
  for (;;) {
    int status;
    pid_t foo = wait (&status);
    if (foo < 0)
      sys_panic ("wait");
    if (foo == pid)
      break;
  }
}

static void run (char **tokens) {
  pid_t pid = must_fork ();
  if (0 == pid)
    must_execvp (tokens);
  else
    wait_for (pid);
}

// Run execvp on tokens, capturing stdout onto the pool.
static void capture (char **tokens) {
  printf ("capture");
  unsigned i;
  for (i = 0; tokens[i]; ++i)
    printf (" [%s]", tokens[i]);
  printf ("\n");
  int fds[2];
  must_pipe (fds);
  pid_t pid = must_fork ();
  if (0 == pid) {
    must_redirect (1, fds[1]);
    must_close (fds[0]);
    must_execvp (tokens);
  }
  else {
    must_close (fds[1]);
    FILE *f = fdopen (fds[0], "r");
    if (NULL == f)
      sys_panic ("fdopen");
    int c;
    strings_append (pool + pool_ptr);
    while (EOF != (c = getc (f))) {
      printf ("got %c [%d]\n", c, c);
      enpool (c);
    }
    enpool ('\0');
    must_not_ferror (f, NULL);
    must_fclose (f, NULL);
    wait_for (pid);
  }
}

static void empty_output (int capturing) {
  if (capturing) {
    strings_append (pool + pool_ptr);
    enpool ('\0');
  }
}

static void cd (int capturing, char **tokens) {
  const char *dir;
  empty_output (capturing);
  if (NULL == tokens[1]) {
    dir = env_lookup (environ, "HOME");
    if (NULL == dir) {
      fprintf (stderr, "cd: no $HOME\n");
      return;
    }
  } else if (NULL == tokens[2]) 
    dir = tokens[1];
  else {
    fprintf (stderr, "usage: cd directory\n");
    return;
  }
  if (chdir (dir) < 0)
    sys_panic (dir);
}

static void run_parsed (int capturing, char **argv) {
  fflush (stdout);
  if (NULL == argv[0])
    empty_output (capturing);
  else if (0 == strcmp ("cd", argv[0]))
    cd (capturing, argv);
  else if (capturing)
    capture (argv);
  else
    run (argv);
}

// Parsing

static const char *scan_quote (const char *s) {
  strings_append (pool + pool_ptr);
  for (; '\'' != *s; ++s)
    switch (*s) {
    case '\0':
      panic ("Unterminated '"); // XXX shouldn't panic
      break;
    // XXX add escaping
    default:
      enpool (*s);
    }
  ++s;
  enpool ('\0');
  return s;
}

static const char *scan_nonblank (const char *s) {
  strings_append (pool + pool_ptr);
  for (; *s && !isspace (*s); ++s)
    enpool (*s);
  enpool ('\0');
  return s;
}

static void split_by_whitespace (const char *s) {
  printf ("split_by_whitespace [%s]\n", s);
  for (;;) {
    while (isspace (*s))
      ++s;
    if ('\0' == *s) {
      strings_append (NULL);
      return;
    }
    s = scan_nonblank (s);
  }
}

static const char *split (const char *s);

static const char *scan_braces (const char *s) {
  printf ("scan_braces [%s]\n", s);
  unsigned pool_start = pool_ptr;
  unsigned strings_start = strings_ptr;
  const char *after = split (s);
  if ('}' != *after)
    panic ("Expected '}'");
  ++after;
  printf ("after [%s]\n", after);
  run_parsed (1, strings + strings_start);
  unsigned output_start = strings_ptr;
  strings_dump ();
  split_by_whitespace (strings[strings_ptr - 1]);
  collapse (pool_start, strings_start, output_start);
  return after;
}

// Scan the next part of s, if any, returning a pointer after it.
// Zero or more strings may be enpooled and pushed in consequence.
// If there's no next part, push NULL onto strings.
static const char *scan_part (const char *s) {
  while (isspace (*s))
    ++s;
  switch (*s) {
    case '\0': 
    case '}':  strings_append (NULL); return s;
    case '\'': return scan_quote (s + 1); //XXX consider nonblank after
    case '{':  return scan_braces (s + 1);
    default:   return scan_nonblank (s);
  }
}

// Split s into tokens, pushing pointers to their enpooled copies onto
// strings. Return a pointer after the successfully parsed head.
static const char *split (const char *s) {
  do {
    s = scan_part (s);
  } while (0 == strings_ptr || NULL != strings[strings_ptr-1]);
  return s;
}

// Parse and run command, with output to stdout.
static void interpret (const char *command) {
  const char *tail = split (command);
  if ('\0' != *tail)
    panic ("Bad syntax");  // XXX don't panic
  run_parsed (0, strings);
}

static void sh (void) {
  char line[64*1024];
  for (;;) {
    printf ("$ ");
    fflush (stdout);
    if (NULL == fgets (line, sizeof line, stdin))
      break;
    trim_newline (line);
    reclaim_strings ();
    interpret (line);
  }
  must_not_ferror (stdin, NULL);
}

void real_main (int argc, char *const argv[]) {
  (void) argv;
  if (1 == argc)
    sh ();
  else
    usage ();
}
