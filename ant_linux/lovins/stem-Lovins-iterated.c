/**************************************************************************
 *
 * stem-Lovins-iterated.c 
 * Stem STDIN text with the iterated Lovins stemmer.
 * Copyright 1997 by Gordon Paynter (gwp@cs.waikato.ac.nz)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 **************************************************************************/

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "stem.h"

void iterated_stem( char *lword ) {
  char length = lword[0];
  char last = lword[length];
  stem(lword);
  while ((length > lword[0]) || (last != lword[length])) {
    length = lword[0];
    last = lword[length];
    stem(lword);
  }
}

void main() 
{
  /* the stem algorithm takes a string with 
     the length in the first byte as input.
     this is the lword variable.  word is a 
     pointer to the start of the string in that 
     variable--the second character--for use in 
     satring operations. */

  char lword[1000];
  char *word = &lword[1];

  int length = 0;
  int c;
  int going = 1;
  int case_offset = (int) 'A' - 'a';

  while (going) {

    c = getchar();
    if (((char) c >= 'A') && ((char) c <= 'Z'))
      c -= case_offset;

    if (c == EOF) {
      // we've finished
      going = 0;

    } else if (((char) c >= 'a') && ((char) c <= 'z')) {
      // add the new character to the word
      word[length++] = (char) c;
      assert(length < 1000);

    } else if (length > 255) {
      // the word is too long for the stemmer to handle, output it
      word[length] = (char) 0;
      printf("%s%c", word, c);
      length = 0;

    } else if (length > 0) {
      // the word is the correct length to be stemmed
      lword[0] = (char) length;
      iterated_stem( lword );
      word[lword[0]] = (char) 0;
      printf("%s%c", word, c);
      length = 0;

    } else {
      // there is no word to be stemmed
      printf("%c", c);
    }
  }
}
