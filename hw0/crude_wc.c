/*
 * Kernighan & Ritchie - The C Programming Language, 2nd. ed.
 *
 * Section 1.5.4: Word Counting... "This is a bare-bones version of the UNIX program wc"
 *
 */


# include <stdio.h>

#define IN 1  /* inside a word */
#define OUT 0  /* outside a word */


/* count lines, words, and characters in input */
int main() {

  int c, nl, nw, nc, state;

  state = OUT;
  nl = nw = nc = 0;

  while ((c = getchar()) != EOF) {
    ++nc;
    if (c == '\n')
      ++nl;
    if (c == ' ' || c == '\n' || c == '\t')
      state = OUT;
    else if (state == OUT) {
      state = IN;
      ++nw;
    }
  }
  printf("%d %d %d\n" , nl, nw, nc);
}