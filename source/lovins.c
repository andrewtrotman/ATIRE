/*
	LOVINS.C
	--------
	OK, I didn't want to do this but it looks like there isn't much choice.  This code
	was originally written by Linh Huynh (linh@kbs.citri.edu.au) (C) 1994 and released
	under GPL 2 with the following attached:

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	It was modified by Eibe Frank (eibe@cs.waikato.ac.nz) in Jan 2001 and I found it in
	the Kea archives at the University of Waikato in New Zealand.

	I (Andrew Trotman, andrew@cs.otago.ac.nz) got it in 2009.  It came with tons of crap 
	that has been deleted (if you want the original its in the Kea archives).  It didn't 
	use function prototypes and used Pascal strings (with a length byte at the start).  
	I've removed the crap, made it pass through the Microsoft C/C++ compiler
	and converted into C NULL-termunated strngs.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pragma.h"
#include "lovins.h"

#define MIN_STEM_LENGTH 2
#define MAX_ENDING_SIZE 11
#define PREDEFINED_SIZE (MAX_ENDING_SIZE + MIN_STEM_LENGTH)
#define EOS     '\0'


/*****************************************************************************
*
*   Transformational Rules used in Recoding Stem Terminations
*
*   Parameter in functions:
*       ch : the second last character of the old ending
*            (after removing one of double consonants) 
*
****************************************************************************/
static int aio(char ch) { return ((ch != 'a') && (ch != 'i') && (ch != 'o')); } /* Rule number 9     */
static int s(char ch) { return (ch != 's'); } /* Rule Number 24    */
static int pt(char ch) { return ((ch != 'p') && (ch != 't')); } /* Rule number 28    */
static int m(char ch) { return (ch != 'm'); } /* Rule number 30    */
static int n(char ch) { return (ch != 'n'); } /* Rule number 32    */

/**************************************************************************
*
*  Context-sensitive rules associated with certain endings.
*
*  The notations of the rules are the same as in Lovins' paper
*  except that rule A is replaced by a NULL in the data structure.
*
*  Function parameters:
*      - stem_length: possible stem length
*      - end        : pointer points to the end of the possible stem.
*
*************************************************************************/
#pragma ANT_PRAGMA_UNUSED_PARAMETER
static int B(int stem_length, char *end) { return (stem_length >= 3); }
#pragma ANT_PRAGMA_UNUSED_PARAMETER
static int C(int stem_length, char *end) { return (stem_length >= 4); }
#pragma ANT_PRAGMA_UNUSED_PARAMETER
static int D(int stem_length, char *end) { return (stem_length >= 5); }
#pragma ANT_PRAGMA_UNUSED_PARAMETER
static int E(int stem_length, char *end) { return (*end != 'e'); }
static int F(int stem_length, char *end) { return ((stem_length >= 3) && (*end != 'e')); }
static int G(int stem_length, char *end) { return ((stem_length >= 3) && (*end == 'f')); }
#pragma ANT_PRAGMA_UNUSED_PARAMETER
static int H(int stem_length, char *end) { return ((*end == 't') || ((*end == 'l') && (*(end - 1) == 'l'))); }
#pragma ANT_PRAGMA_UNUSED_PARAMETER
static int I(int stem_length, char *end) { return ((*end != 'o') && (*end != 'e')); }
#pragma ANT_PRAGMA_UNUSED_PARAMETER
static int J(int stem_length, char *end) { return ((*end != 'a') && (*end != 'e')); }
static int K(int stem_length, char *end) { return ((stem_length >= 3) && ((*end == 'l') || (*end == 'i') || ((*end == 'e') && (*(end - 2) == 'u')))); }
#pragma ANT_PRAGMA_UNUSED_PARAMETER
static int L(int stem_length, char *end) { return (*end == 's') ? (*(end - 1) == 'o') : ((*end != 'u') && (*end != 'x')); }
#pragma ANT_PRAGMA_UNUSED_PARAMETER
static int M(int stem_length, char *end) { return ((*end != 'a') && (*end != 'c') && (*end != 'e') && (*end != 'm')); }
static int N(int stem_length, char *end) { return (stem_length >= 3) ? (*(end - 2) == 's') ? (stem_length >= 4) : 1 : 0; }
#pragma ANT_PRAGMA_UNUSED_PARAMETER
static int O(int stem_length, char *end) { return ((*end == 'l') || (*end == 'i')); }
#pragma ANT_PRAGMA_UNUSED_PARAMETER
static int P(int stem_length, char *end) { return (*end != 'c'); }
static int Q(int stem_length, char *end) { return ((stem_length >= 3) && (*end != 'l') && (*end != 'n')); }
#pragma ANT_PRAGMA_UNUSED_PARAMETER
static int R(int stem_length, char *end) { return ((*end == 'n') || (*end == 'r')); }
#pragma ANT_PRAGMA_UNUSED_PARAMETER
static int S(int stem_length, char *end) { return (*end == 't') ? (*(end - 1) != 't') : ((*end == 'r') && (*(end - 1) == 'd')); }
#pragma ANT_PRAGMA_UNUSED_PARAMETER
static int T(int stem_length, char *end) { return (*end == 't') ? (*(end - 1) != 'o') : (*end == 's'); }
#pragma ANT_PRAGMA_UNUSED_PARAMETER
static int U(int stem_length, char *end) { return ((*end == 'l') || (*end == 'm') || (*end == 'n') || (*end == 'r')); }
#pragma ANT_PRAGMA_UNUSED_PARAMETER
static int V(int stem_length, char *end) { return (*end == 'c'); }
#pragma ANT_PRAGMA_UNUSED_PARAMETER
static int W(int stem_length, char *end) { return ((*end != 's') && (*end != 'u')); }
static int X(int stem_length, char *end) { return (*end == 'e') ? ((stem_length >= 3) && (*(end - 2) == 'u')) : ((*end == 'l') || (*end == 'i')); }
#pragma ANT_PRAGMA_UNUSED_PARAMETER
static int Y(int stem_length, char *end) { return ((*end == 'n') && (*(end - 1) == 'i')); }
#pragma ANT_PRAGMA_UNUSED_PARAMETER
static int Z(int stem_length, char *end) { return (*end != 'f'); }
static int AA(int stem_length, char *end)
{
char p1, p2;
p1 = *end;
p2 = *(end - 1);

if (p1 == 'h')
	return ((p2 == 'p') || (p2 == 't'));
else if (p1 == 'r')
	return ((p2 == 'e') || (p2 == 'o'));
else if (p1 == 's')
	return (p2 == 'e');
else
	return ((p1 == 'd') || (p1 == 'f') || (p1 == 'l') || (p1 == 't'));
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
static int BB (int stem_length, char *end) { return ((stem_length > 2) && (strncmp (end - 2, "met", 3) != 0) && ((stem_length < 4) || (strncmp (end - 3, "ryst", 4) != 0))) ? 1 : 0; } /* CHANGED by EF */ 
#pragma ANT_PRAGMA_UNUSED_PARAMETER
static int CC (int stem_length, char *end) { return (*end == 'l'); }

/* =========================================================================
 *
 *   Data structure for recoding and the method for recoding the stem. 
 *
 * ========================================================================= */

class Recode_Rules
{
public:
	const char *old_end;		/* old ending */
	const char *new_end;		/* new ending */
	char old_offset;	/* length of the old ending - 1 */
	int (*cond)(char);		/* condition rule */
	char end_group;		/* signal the end of the group */
} ;

/*  
   Stem terminations are divided into groups having the same last character
*/
static Recode_Rules Rules[] =
{
"uad",  "uas",   2, NULL, 0,
"vad",  "vas",   2, NULL, 0,
"cid",  "cis",   2, NULL, 0,
"lid",  "lis",   2, NULL, 0,
"erid", "eris",  3, NULL, 0,
"pand", "pans",  3, NULL, 0,
"end",  "ens",   2, s,    0,
"end",  "ens",   2, m,    0,
"ond",  "ons",   2, NULL, 0,
"lud",  "lus",   2, NULL, 0,
"rud",  "rus",   2, NULL, 1,

"ul",   "l",     1, aio,  1,

"istr", "ister", 3, NULL, 0,
"metr", "meter", 3, NULL, 0,
"her",  "hes",   2, pt,   1,

"urs",  "ur",    2, NULL, 1,

"uct",  "uc",    2, NULL, 0,
"umpt", "um",    3, NULL, 0,
"rpt",  "rb",    2, NULL, 0,
"mit",  "mis",   2, NULL, 0,
"ert",  "ers",   2, NULL, 0,
"et",   "es",    1, n,    0,
"yt",   "ys",    1, NULL, 1,

"iev",  "ief",   2, NULL, 0,
"olv",  "olut",  2, NULL, 1,

"bex",  "bic",   2, NULL, 0,
"dex",  "dic",   2, NULL, 0,
"pex",  "pic",   2, NULL, 0,
"tex",  "tic",   2, NULL, 0,
"ax",   "ac",    1, NULL, 0,
"ex",   "ec",    1, NULL, 0,
"ix",   "ic",    1, NULL, 0,
"lux",  "luc",   2, NULL, 1,

"yz", "ys", 1, NULL, 1
};

class Last_Char_Node
{
public:
	char c;									/* The last character */
	Last_Char_Node *left;		/* used in balanced   */
	Last_Char_Node *right;		/* binary tree        */
	Recode_Rules *pt;			/* location of approriate group */
} ;

/*------------
       s
     /   \
    l     x
   / \   / \
  d   r t   z
         \
          v
---------------*/
static Last_Char_Node pr[] =
{
'd', NULL,   NULL,   Rules,
'l', pr,     pr + 2, Rules + 11,
'r', NULL,   NULL,   Rules + 12,
's', pr + 1, pr + 6, Rules + 15,
't', NULL,   pr + 5, Rules + 16,
'v', NULL,   NULL,   Rules + 23,
'x', pr + 4, pr + 7, Rules + 25,
'z', NULL,   NULL,   Rules + 33,
} ;


/*****************************************************************************
 *
 *  Recode the stem according to the transformation rules.
 *
 *****************************************************************************/

static void recode_stem(char *stem_end)
{
static Last_Char_Node *root = pr + 3;
Last_Char_Node *p_last = root;
Recode_Rules *rule;				/* points to the transformation list */
char *h;							/* points to start of possible ending */
char ch;							/* last character of the old ending  */
char ch2;

h = stem_end - 1;
ch = *stem_end;
if (*h == ch)			/* Check for double consonant        */
	{
	if (strchr("bdglmnprst", ch) != NULL)
		{
		*stem_end = EOS;
		stem_end = h;
		}
	}

do				/* Check for the last character       */
	{
	ch2 = p_last->c;
	if (ch == ch2)
		break;
	else if (ch > ch2)
		p_last = p_last->right;
	else
		p_last = p_last->left;
	}
while (p_last != NULL);

if (p_last != NULL)		/* Check for the rest of suffix list */
	{
	rule = p_last->pt;
	for (;;)
		{
		h = stem_end - rule->old_offset;
		if (strcmp (h, rule->old_end) == 0)
			{
			if (!rule->cond || (*rule->cond) (*(h - 1)))
				{
				(void) strcpy (h, rule->new_end);
				break;
				}
			}
		if (rule->end_group)
			break;
		rule++;
		}
	}
}

/* ======================================================================

 *    List of endings and the method to remove the ending 
 *
 * ======================================================================*/

/************ Data structures for list of endings  ********************/

class Ending_List
{
public:
	const char *ending;				/* old ending                */
	int (*cond) (int, char *);				/* conditional rule          */
	signed char left_offset;	/* used to find the siblings */
	signed char right_offset;	/* in balanced binary tree   */
} ;

/*
   The original list of endings is re-organised into groups having
   the same length and the same first character.
 */

static Ending_List List[] =
{
"a", NULL, 0, 0,

"ae", NULL, 0, 0,
"al", BB, -1, 2,
"ar", X, 0, 0,
"as", B, -1, 0,

"acy", NULL, 0, 1,
"age", B, 0, 0,
"aic", NULL, -2, 1,
"als", BB, 0, 0,
"ant", B, -2, 2,
"ars", O, 0, 0,
"ary", F, -1, 2,
"ata", NULL, 0, 0,
"ate", NULL, -1, 0,

"able", NULL, 0, 1,
"ably", NULL, 0, 0,
"ages", B, -2, 2,
"ally", B, 0, 0,
"ance", B, -1, 1,
"ancy", B, 0, 0,
"ants", B, -4, 4,
"aric", NULL, 0, 0,
"arly", K, -1, 1,
"ated", I, 0, 0,
"ates", NULL, -2, 2,
"atic", B, 0, 0,
"ator", NULL, -1, 0,

"acies", NULL, 0, 0,
"acity", NULL, -1, 1,
"aging", B, 0, 0,
"aical", NULL, -2, 2,
"alism", B, 0, 0,        /* CHANGED by EF */
"alist", NULL, -1, 0,    /* CHANGED by EF */
"ality", NULL, -3, 3,
"alize", NULL, 0, 1,
"allic", BB, 0, 0,
"anced", B, -2, 2,
"ances", B, 0, 0,
"antic", C, -1, 0,
"arial", NULL, -6, 6,
"aries", NULL, 0, 1,
"arily", NULL, 0, 0,
"arity", B, -2, 2,
"arize", NULL, 0, 0,
"aroid", NULL, -1, 0,
"ately", NULL, -3, 3,
"ating", I, 0, 1,
"ation", B, 0, 0,
"ative", NULL, -2, 2,
"ators", NULL, 0, 0,
"atory", NULL, -1, 1,
"ature", E, 0, 0,

"aceous", NULL, 0, 1,
"acious", B, 0, 0,
"action", G, -2, 2,
"alness", NULL, 0, 0,
"ancial", NULL, -1, 1,
"ancies", NULL, 0, 0,
"ancing", B, -4, 4,
"ariser", NULL, 0, 0,
"arized", NULL, -1, 1,
"arizer", NULL, 0, 0,
"atable", NULL, -2, 2,
"ations", B, 0, 0,
"atives", NULL, -1, 0,

"ability", NULL, 0, 1,
"aically", NULL, 0, 0,
"alistic", B, -2, 2,
"alities", NULL, 0, 0,
"ariness", E, -1, 0,
"aristic", NULL, -3, 3,
"arizing", NULL, 0, 1,
"ateness", NULL, 0, 0,
"atingly", NULL, -2, 2,
"ational", B, 0, 0,
"atively", NULL, -1, 1,
"ativism", NULL, 0, 0,

"ableness", NULL, 0, 1,
"arizable", NULL, 0, 0,

"allically", C, 0, 0,
"antaneous", NULL, -1, 1,
"antiality", NULL, 0, 0,
"arisation", NULL, -2, 2,
"arization", NULL, 0, 0,
"ationally", B, -1, 1,
"ativeness", NULL, 0, 0,

"antialness", NULL, 0, 0,
"arisations", NULL, -1, 1,
"arizations", NULL, 0, 0,

"alistically", B, 0, 1,
"arizability", NULL, 0, 0,

"e", NULL, 0, 0,

"ed", E, 0, 0,
"en", F, -1, 1,
"es", E, 0, 0,

"eal", Y, 0, 0,
"ear", Y, -1, 1,
"ely", E, 0, 0,
"ene", E, -2, 2,
"ent", C, 0, 0,
"ery", E, -1, 1,
"ese", NULL, 0, 0,

"ealy", Y, 0, 1,
"edly", E, 0, 0,
"eful", NULL, -2, 1,
"eity", NULL, 0, 0,
"ence", NULL, -2, 2,
"ency", NULL, 0, 0,
"ened", E, -1, 2,
"enly", E, 0, 0,
"eous", NULL, -1, 0,

"early", Y, 0, 1,
"ehood", NULL, 0, 0,
"eless", NULL, -2, 2,
"elily", NULL, 0, 0, /* CHANGED by EF */
"ement", NULL, -1, 0,
"enced", NULL, -3, 3,
"ences", NULL, 0, 1,
"eness", E, 0, 0,
"ening", E, -2, 2,
"ental", NULL, 0, 0,
"ented", C, -1, 1,
"ently", NULL, 0, 0,

"eature", Z, 0, 0,
"efully", NULL, -1, 1,
"encies", NULL, 0, 0,
"encing", NULL, -2, 2,
"ential", NULL, 0, 0,
"enting", C, -1, 1,
"entist", NULL, 0, 1,
"eously", NULL, 0, 0,

"elihood", E, 0, 1,
"encible", NULL, 0, 0,
"entally", NULL, -2, 2,
"entials", NULL, 0, 0,
"entiate", NULL, -1, 1,
"entness", NULL, 0, 0,

"entation", NULL, 0, 0,
"entially", NULL, -1, 1,
"eousness", NULL, 0, 0,

"eableness", E, 0, 1,
"entations", NULL, 0, 0,
"entiality", NULL, -2, 2,
"entialize", NULL, 0, 0,
"entiation", NULL, -1, 0,

"entialness", NULL, 0, 0,

"ful", NULL, 0, 0,

"fully", NULL, 0, 0,

"fulness", NULL, 0, 0,

"hood", NULL, 0, 0,

"i", NULL, 0, 0,

"ia", NULL, 0, 0,
"ic", NULL, -1, 1,
"is", NULL, 0, 0,

"ial", NULL, 0, 0,
"ian", NULL, -1, 1,
"ics", NULL, 0, 1,
"ide", L, 0, 0,
"ied", NULL, -3, 3,
"ier", NULL, 0, 0,
"ies", P, -1, 0,
"ily", NULL, -1, 1,
"ine", M, 0, 0,
"ing", N, -5, 5,
"ion", Q, 0, 0,
"ish", C, -1, 1,
"ism", B, 0, 1,
"ist", NULL, 0, 0,
"ite", AA, -3, 3,
"ity", NULL, 0, 0,
"ium", NULL, -1, 0,
"ive", NULL, -1, 1,
"ize", F, 0, 0,

"ials", NULL, 0, 0,
"ians", NULL, -1, 0,
"ible", NULL, -1, 1,
"ibly", NULL, 0, 0,
"ical", NULL, -2, 2,
"ides", L, 0, 0,
"iers", NULL, -1, 1,
"iful", NULL, 0, 0,
"ines", M, -4, 4,
"ings", N, 0, 0,
"ions", B, -1, 1,
"ious", NULL, 0, 0,
"isms", B, -2, 2,
"ists", NULL, 0, 0,
"itic", H, -1, 1,
"ized", F, 0, 1,
"izer", F, 0, 0,

"ially", NULL, 0, 0,
"icant", NULL, -1, 1,
"ician", NULL, 0, 0,
"icide", NULL, -2, 2,
"icism", NULL, 0, 0,
"icist", NULL, -1, 0,
"icity", NULL, -3, 3,
"idine", I, 0, 1,
"iedly", NULL, 0, 0,
"ihood", NULL, -2, 2,
"inate", NULL, 0, 0,
"iness", NULL, -1, 0,
"ingly", B, -6, 6,
"inism", J, 0, 1,
"inity", CC, 0, 0,
"ional", NULL, -2, 2,
"ioned", NULL, 0, 0,
"ished", NULL, -1, 0,
"istic", NULL, -3, 3,
"ities", NULL, 0, 1,
"itous", NULL, 0, 0,
"ively", NULL, -2, 2,
"ivity", NULL, 0, 0,
"izers", F, -1, 1,
"izing", F, 0, 0,

"ialist", NULL, 0, 0,
"iality", NULL, -1, 1,
"ialize", NULL, 0, 0,
"ically", NULL, -2, 2,
"icance", NULL, 0, 0,
"icians", NULL, -1, 1,
"icists", NULL, 0, 0,
"ifully", NULL, -4, 4,
"ionals", NULL, 0, 0,
"ionate", D, -1, 1,
"ioning", NULL, 0, 0,
"ionist", NULL, -2, 2,
"iously", NULL, 0, 0,
"istics", NULL, -1, 1,
"izable", E, 0, 0,

"ibility", NULL, 0, 0,
"icalism", NULL, -1, 1,
"icalist", NULL, 0, 1,
"icality", NULL, 0, 0,
"icalize", NULL, -3, 3,
"ication", G, 0, 0,
"icianry", NULL, -1, 0,
"ination", NULL, -1, 1,
"ingness", NULL, 0, 0,
"ionally", NULL, -5, 5,
"isation", NULL, 0, 0,
"ishness", NULL, -1, 1,
"istical", NULL, 0, 1,
"iteness", NULL, 0, 0,
"iveness", NULL, -3, 3,
"ivistic", NULL, 0, 0,
"ivities", NULL, -1, 0,
"ization", F, -1, 1,
"izement", NULL, 0, 0,

"ibleness", NULL, 0, 0,
"icalness", NULL, -1, 1,
"ionalism", NULL, 0, 0,
"ionality", NULL, -2, 2,
"ionalize", NULL, 0, 0,
"iousness", NULL, -1, 1,
"izations", NULL, 0, 0,

"ionalness", NULL, 0, 1,
"istically", NULL, 0, 0,
"itousness", NULL, -2, 2,
"izability", NULL, 0, 0,
"izational", NULL, -1, 0,

"izationally", B, 0, 0,

"ly", B, 0, 0,

"less", NULL, 0, 1,
"lily", NULL, 0, 0,

"lessly", NULL, 0, 0,

"lessness", NULL, 0, 0,

"ness", NULL, 0, 0,

"nesses", NULL, 0, 0,

"o", NULL, 0, 0,

"on", S, 0, 1,
"or", T, 0, 0,

"oid", NULL, 0, 0,
"one", R, -1, 1,
"ous", NULL, 0, 0,

"ogen", NULL, 0, 0,

"oidal", NULL, 0, 0,
"oides", NULL, -1, 2,
"otide", NULL, 0, 0,
"ously", NULL, -1, 0,

"oidism", NULL, 0, 0,

"oidally", NULL, 0, 1,
"ousness", NULL, 0, 0,

"s", W, 0, 0,

"s'", NULL, 0, 0,

"um", U, 0, 1,
"us", V, 0, 0,

"ward", NULL, 0, 1,
"wise", NULL, 0, 0,

"y", B, 0, 0,

"yl", R, 0, 0,

"ying", B, 0, 1,
"yish", NULL, 0, 0,

"'s", NULL, 0, 0,
};

class First_Char_Node
{
public:
	char c;							/* First character  */
	First_Char_Node *left;			/* used in balanced */
	First_Char_Node *right;			/* binary tree      */
	Ending_List *ptr[11];			/* the approriate location */
} ;

static First_Char_Node First[] =
{
'\'', NULL, NULL, {NULL, List + 293, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
'a', First, NULL, {List, List + 2, List + 9, List + 20, List + 39, List + 58, List + 70, List + 77, List + 82, List + 87, List + 89},
'e', First + 1, First + 4, {List + 91, List + 93, List + 98, List + 106, List + 116, List + 126, List + 133, List + 138, List + 142, List + 145, NULL},
'f', NULL, NULL, {NULL, NULL, List + 146, NULL, List + 147, NULL, List + 148, NULL, NULL, NULL, NULL},
'h', First + 3, First + 5, {NULL, NULL, NULL, List + 149, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
'i', NULL, NULL, {List + 150, List + 152, List + 163, List + 181, List + 202, List + 222, List + 239, List + 252, List + 258, NULL, List + 261},
'l', First + 2, First + 10, {NULL, List + 262, NULL, List + 263, NULL, List + 265, NULL, List + 266, NULL, NULL, NULL},
'n', NULL, NULL, {NULL, NULL, NULL, List + 267, NULL, List + 268, NULL, NULL, NULL, NULL, NULL},
'o', First + 7, First + 9, {List + 269, List + 270, List + 273, List + 275, List + 277, List + 280, List + 281, NULL, NULL, NULL, NULL},
's', NULL, NULL, {List + 283, List + 284, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
'u', First + 8, First + 12, {NULL, List + 285, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
'w', NULL, NULL, {NULL, NULL, NULL, List + 287, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
'y', First + 11, NULL, {List + 289, List + 290, NULL, List + 291, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
};

/******************************************************************************
 *
 *  Look for a match in the suffix list.
 *  Return the pointer to the end of the new stem if there is a match.
 *  Otherwise, return the pointer to the end of the original word.
 *
 *  Method:
 *      * Search for the first character in the suffix list.
 *      * If there is match, search for the rest of the suffix list.
 *         
 *****************************************************************************/
char *remove_ending (char *word, int w_len)
{
static First_Char_Node *root = First + 6;	/* the root of binary tree is 'l' */
First_Char_Node *p_first;	/* Points to the first character of the possible suffix */
Ending_List *p_list;		/* Points to the List of Endings      */
char *suffix_start;		/* Points to start of possible suffix */
char *stem_end;			/* Points to the end of possible stem */
char ch1, ch2;
int s_len;					/* Possible stem length               */
int e_offset;				/* Offset from the end to the start of possible suffix */
int not_done = 1, cmp;

stem_end = word + w_len - 1;
if (w_len > PREDEFINED_SIZE)	/* find the start of suffix   */
	suffix_start = word + w_len - MAX_ENDING_SIZE;
else
	suffix_start = word + MIN_STEM_LENGTH;

ch1 = *suffix_start;
do				/* Search for the first character     */
	{
	p_first = root;
	do
		{
		ch2 = p_first->c;
		if (ch1 == ch2)
			break;
		else if (ch1 > ch2)
			p_first = p_first->right;
		else
			p_first = p_first->left;
		}
	while (p_first != NULL);

	if (p_first != NULL)	/* Search for the rest */
		{
		e_offset = (int)(stem_end - suffix_start);
		if ((p_list = p_first->ptr[e_offset]) != NULL)
			{
			for (;;)		/* no need to compare the first char  */
				{
				cmp = strcmp (suffix_start + 1, p_list->ending + 1);
				if (cmp > 0)
					if (p_list->right_offset)
						p_list += p_list->right_offset;
					else
						break;
				else if (cmp < 0)
					if (p_list->left_offset)
						p_list += p_list->left_offset;
					else
						break;
				else
					{
					s_len = (int)(suffix_start - word);
					if (!p_list->cond || (*p_list->cond) (s_len, suffix_start - 1))
						{
						*suffix_start = EOS;
						stem_end = suffix_start - 1;
						not_done = 0;
						}
					break;
					}
				}
			}
		}
	suffix_start++;
	}
while (not_done && ((ch1 = *suffix_start) != EOS));

return stem_end;
}

/**************************************************************************/

/*
	LOVINS_STEM ()
	--------------
*/
size_t lovins_stem(char *term, char *destination)
{
int length;
char *stem_end;

length = (int)strlen(strcpy(destination, term));
if (length > 2)		// This constraint comes from the original code and if you remove it then it crashes!
	{
	stem_end = remove_ending(destination, length);
	recode_stem(stem_end);
	}

//printf("%s : %s\n", term, destination);

return strlen(destination);
}

