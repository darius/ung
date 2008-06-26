#include "lib.h"

const char *usage_message = "diff file1 file2";

typedef const char *Line;

typedef struct Buffer Buffer;
struct Buffer {
  const char *filename;
  size_t      capacity;
  size_t      size;
  char*       chars;
  size_t      n_lines;
  Line*       lines;
};

/* Append c to the buffer. */
static void append_char (Buffer *b, char c) {
  if (b->size == b->capacity) {
    b->capacity = must_grow (b->capacity);
    b->chars = must_realloc (b->chars, b->capacity);
  }
  b->chars[b->size++] = c;
}

/* End the buffer's current line (which should be nonempty). */
static void end_line (Buffer *b) {
  append_char (b, '\0');
  b->n_lines++;
}

/* Append c to the buffer. */
static void append (Buffer *b, char c) {
  append_char (b, c);
  if ('\n' == c)
    end_line (b);
}

/* Fill in b->lines with the start of each line in b. */
static void index_lines (Buffer *b) {
  b->lines = must_malloc (b->n_lines * sizeof b->lines[0]);
  {
    size_t L;
    const char *s = b->chars;
    for (L = 0; L < b->n_lines; ++L) {
      b->lines[L] = s;
      s = strchr (s, '\n'); /* XXX still assumes no nulls in the input */
      assert ((s && !s[1]) || L+1 == b->n_lines);
      s += 2;
    }
  }
}

/* Append the contents of ::in to the buffer. */
static void snarf (Buffer *b, FILE *in, const char *opt_filename) {
  int c;
  b->filename = opt_filename;
  while (EOF != (c = getc (in)))
    append (b, c);
  must_not_ferror (in, opt_filename);
  if (0 < b->size && '\0' != b->chars[b->size - 1])
    end_line (b);
  index_lines (b);
}

static void load (Buffer *b, const char *filename) {
  FILE *file = must_fopen (filename, "r");
  snarf (b, file, filename);
  must_fclose (file, filename);
}

static unsigned line_cost (Line line1, Line line2) {
  return 0 == strcmp (line1, line2) ? 0 : 1;
}

static Buffer b1;
static Buffer b2;

/* cost_cells[(b2.n_lines+1) * i + j] == 0 if position i of b1 and j of b2
   do not yet have a diff-cost; otherwise == 1 + the cost. */
static unsigned *cost_cells;

static void allot_costs (void) {
  unsigned n = (b1.n_lines+1) * (b2.n_lines+1);
  cost_cells = must_malloc (n * sizeof cost_cells[0]);
  memset (cost_cells, 0, n * sizeof cost_cells[0]);
}

static unsigned *cost_cell (unsigned line1, unsigned line2) {
  assert (line1 <= b1.n_lines);
  assert (line2 <= b2.n_lines);
  return cost_cells + (b2.n_lines+1) * line1 + line2;
}

static unsigned compute_cost (unsigned coord1, unsigned coord2);

static unsigned get_cost (unsigned coord1, unsigned coord2) {
  unsigned *cell = cost_cell (coord1, coord2);
  unsigned cc = *cell;
  if (0 == cc)
    *cell = cc = compute_cost (coord1, coord2) + 1;
  return cc - 1;
}

static unsigned min (unsigned a, unsigned b) {
  return a <= b ? a : b;
}

static unsigned compute_cost (unsigned coord1, unsigned coord2) {
  if (0 == coord1) return coord2;
  if (0 == coord2) return coord1;
  {
    unsigned subst_cost = line_cost (b1.lines[coord1-1], b2.lines[coord2-1]);
    unsigned cost_for_subst  = get_cost (coord1-1, coord2-1) + subst_cost;
    unsigned cost_for_delete = get_cost (coord1-1, coord2)   + 1;
    unsigned cost_for_insert = get_cost (coord1,   coord2-1) + 1;
    return min (min (cost_for_subst, cost_for_delete), cost_for_insert);
  }
}

static void print_best_edit (unsigned coord1, unsigned coord2);

static void print_insert (unsigned coord1, unsigned coord2) {
  print_best_edit (coord1, coord2-1);
  must_printf ("i %u: %s\n", coord2, b2.lines[coord2-1]);
}

static void print_delete (unsigned coord1, unsigned coord2) {
  print_best_edit (coord1-1, coord2);
  must_printf ("d %u: %s\n", coord1, b1.lines[coord1-1]);
}

static void print_subst (unsigned coord1, unsigned coord2, unsigned cost) {
  print_best_edit (coord1-1, coord2-1);
  if (0 < cost) {
    must_printf ("d %u: %s\n", coord1, b1.lines[coord1-1]);
    must_printf ("i %u: %s\n", coord2, b2.lines[coord2-1]);
  }
}

static void print_best_edit (unsigned coord1, unsigned coord2) {
  if      (0 == coord1 && 0 == coord2) return;
  else if (0 == coord1) print_insert (coord1, coord2);
  else if (0 == coord2) print_delete (coord1, coord2);
  else {
    /* TODO: fix code duplication, collect runs */
    unsigned subst_cost = line_cost (b1.lines[coord1-1], b2.lines[coord2-1]);
    unsigned cost_for_subst  = get_cost (coord1-1, coord2-1) + subst_cost;
    unsigned cost_for_delete = get_cost (coord1-1, coord2)   + 1;
    unsigned cost_for_insert = get_cost (coord1,   coord2-1) + 1;
    unsigned cost = min (min (cost_for_subst, cost_for_delete),
                         cost_for_insert);
    if (cost == cost_for_subst)
      print_subst (coord1, coord2, subst_cost);
    else if (cost == cost_for_delete)
      print_delete (coord1, coord2);
    else
      print_insert (coord1, coord2);
  }
}

static int diff (void) {
  unsigned cost;
  allot_costs ();
  cost = get_cost (b1.n_lines, b2.n_lines);
  print_best_edit (b1.n_lines, b2.n_lines);
  return 0 < cost;
}

void real_main (int argc, char *const argv[]) {
  int i = 1;
  /* TODO: options */

  if (argc - i != 2) 
    usage ();

  load (&b1, argv[i]);
  load (&b2, argv[i+1]);
  exit (diff ());
}
