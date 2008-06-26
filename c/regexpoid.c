#include "regexpoid.h"

/* Match strings against this subset of regular expressions:
     regexp  ::= '^'? (('.' | '\\' literal | literal) '*'?)* '$'?
     literal ::= any nonnull character
   The implementation follows this reformulation of the grammar:
     unit    ::= '.' | '\\' literal |  literal             
     segment ::= unit '*' re
     re      ::=  | '$' | segment | unit re
     regexp  ::= '^'? re           */

static int unit_matches (const char *unit, const char *s) {
  if      (!s[0])                      return 0;
  else if (unit[0] == '.')             return 1;
  else if (unit[0] == '\\' && unit[1]) return unit[1] == s[0];
  else                                 return unit[0] == s[0];
}

/* Return a pointer just past the unit. */
static const char *skip_unit (const char *unit) {
  if      (unit[0] == '\\' && unit[1]) return unit + 2;
  else                                 return unit + 1;
}

static int re_matches (const char *re, const char *s);
static int segment_matches (const char *unit, const char *re, const char *s) {
  for (; unit_matches (unit, s); ++s)
    if (re_matches (re, s))
      return 1;
  return re_matches (re, s);
}

static int re_matches (const char *re, const char *s) {
  if      (!re[0])                 return 1;
  else if (re[0] == '$' && !re[1]) return !s[0];
  else {
    const char *re_tail = skip_unit (re);
    if (re_tail[0] == '*') return segment_matches (re, re_tail + 1, s);
    else                   return (unit_matches (re, s)
                                   && re_matches (re_tail, s + 1));
  }
}

/* Return true iff regexp matches the string s. */
int regexpoid_matches (const char *regexp, const char *s) {
  if (regexp[0] == '^') return re_matches (regexp + 1, s);
  else                  return segment_matches (".", regexp, s);
}
