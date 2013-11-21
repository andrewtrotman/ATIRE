/*
	STOP_WORD.C
	-----------
	We'll use the NCBI wrd_stop stop word list of 313 terms extracted from Medline.
	Its use is unrestricted (see here:http://mbr.nlm.nih.gov/).  The list can be 
	downloaded from here: http://mbr.nlm.nih.gov/Download/2009/WordCounts/wrd_stop

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "str.h"
#include "stop_word.h"

/*
	ANT_STOP_WORD::ISSTOP()
	------------------------
*/
long ANT_stop_word::isstop(const char *term)
{
if (bsearch(&term, ANT_stop_word_list, ANT_stop_word_list_len, sizeof(*ANT_stop_word_list), char_star_star_strcmp) == NULL)
	if (bsearch(&term, extra_stop, extra_stop_length, sizeof(*extra_stop), char_star_star_strcmp) == NULL)
		return 0;

return 1;
}

/*
	ANT_STOP_WORD::ISSTOP()
	------------------------
*/
long ANT_stop_word::isstop(const char *term, long term_len)
{
if (term_len >= len)
	{
	delete [] buffer;
	buffer = new char [term_len + 1];
	len = term_len;
	}
strncpy(buffer, term, term_len);
buffer[term_len] = '\0';

return isstop(buffer);
}

/*
	ANT_STOP_WORD::ANT_STOP_WORD()
	------------------------------
*/
ANT_stop_word::ANT_stop_word()
{
buffer = NULL;
len = 0;
extra_stop = NULL;
extra_stop_length = 0;
}

/*
	ANT_STOP_WORD::~ANT_STOP_WORD()
	-------------------------------
*/
ANT_stop_word::~ANT_stop_word()
{
delete [] buffer;
delete [] extra_stop;
}

/*
	ANT_STOP_WORD::ADDSTOP()
	-------------------------
*/
long ANT_stop_word::addstop(const char *term)
{
char **copy;

copy = new char *[extra_stop_length + 1];
memcpy(copy, extra_stop, sizeof(*extra_stop) * extra_stop_length);

copy[extra_stop_length] = strnew(term);
extra_stop_length++;
delete [] extra_stop;
extra_stop = copy;

/*
	this isn't very efficient so if you have a batch of terms to
	add, add them as a NULL terminated list.
*/
qsort(extra_stop, extra_stop_length, sizeof(*extra_stop), char_star_star_strcmp);

return extra_stop_length;
}

/*
	ANT_STOP_WORD::ADDSTOP()
	-------------------------
*/
long ANT_stop_word::addstop(const char **term)
{
long len, which;
char **copy;

len = 0;
while (term[len] != NULL)
	len++;

if (len != 0)
	{
	copy = new char *[extra_stop_length + len];
	memcpy(copy, extra_stop, sizeof(*extra_stop) * extra_stop_length);


	for (which = 0; which < len; which++)
		copy[extra_stop_length + which] = strnew(term[which]);

	extra_stop_length += len;
	delete [] extra_stop;
	extra_stop = copy;

	qsort(extra_stop, extra_stop_length, sizeof(*extra_stop), char_star_star_strcmp);
	}

return extra_stop_length;
}

/*
	ANT_STOP_WORD_LIST
	-------------------
*/
char *ANT_stop_word::ANT_stop_word_list[] = {
	(char *)"a",
	(char *)"about",
	(char *)"above",
	(char *)"across",
	(char *)"after",
	(char *)"afterwards",
	(char *)"again",
	(char *)"against",
	(char *)"al",
	(char *)"all",
	(char *)"almost",
	(char *)"alone",
	(char *)"along",
	(char *)"already",
	(char *)"also",
	(char *)"although",
	(char *)"always",
	(char *)"am",
	(char *)"among",
	(char *)"amongst",
	(char *)"an",
	(char *)"analyze",
	(char *)"and",
	(char *)"another",
	(char *)"any",
	(char *)"anyhow",
	(char *)"anyone",
	(char *)"anything",
	(char *)"anywhere",
	(char *)"applicable",
	(char *)"apply",
	(char *)"are",
	(char *)"around",
	(char *)"as",
	(char *)"assume",
	(char *)"at",
	(char *)"be",
	(char *)"became",
	(char *)"because",
	(char *)"become",
	(char *)"becomes",
	(char *)"becoming",
	(char *)"been",
	(char *)"before",
	(char *)"beforehand",
	(char *)"being",
	(char *)"below",
	(char *)"beside",
	(char *)"besides",
	(char *)"between",
	(char *)"beyond",
	(char *)"both",
	(char *)"but",
	(char *)"by",
	(char *)"came",
	(char *)"cannot",
	(char *)"cc",
	(char *)"cm",
	(char *)"come",
	(char *)"compare",
	(char *)"could",
	(char *)"de",
	(char *)"dealing",
	(char *)"department",
	(char *)"depend",
	(char *)"did",
	(char *)"discover",
	(char *)"dl",
	(char *)"do",
	(char *)"does",
	(char *)"during",
	(char *)"each",
	(char *)"ec",
	(char *)"ed",
	(char *)"effected",
	(char *)"eg",
	(char *)"either",
	(char *)"else",
	(char *)"elsewhere",
	(char *)"enough",
	(char *)"et",
	(char *)"etc",
	(char *)"ever",
	(char *)"every",
	(char *)"everyone",
	(char *)"everything",
	(char *)"everywhere",
	(char *)"except",
	(char *)"find",
	(char *)"for",
	(char *)"found",
	(char *)"from",
	(char *)"further",
	(char *)"get",
	(char *)"give",
	(char *)"go",
	(char *)"gov",
	(char *)"had",
	(char *)"has",
	(char *)"have",
	(char *)"he",
	(char *)"hence",
	(char *)"her",
	(char *)"here",
	(char *)"hereafter",
	(char *)"hereby",
	(char *)"herein",
	(char *)"hereupon",
	(char *)"hers",
	(char *)"herself",
	(char *)"him",
	(char *)"himself",
	(char *)"his",
	(char *)"how",
	(char *)"however",
	(char *)"hr",
	(char *)"ie",
	(char *)"if",
	(char *)"ii",
	(char *)"iii",
	(char *)"in",
	(char *)"inc",
	(char *)"incl",
	(char *)"indeed",
	(char *)"into",
	(char *)"investigate",
	(char *)"is",
	(char *)"it",
	(char *)"its",
	(char *)"itself",
	(char *)"j",
	(char *)"jour",
	(char *)"journal",
	(char *)"just",
	(char *)"kg",
	(char *)"last",
	(char *)"latter",
	(char *)"latterly",
	(char *)"lb",
	(char *)"ld",
	(char *)"letter",
	(char *)"like",
	(char *)"ltd",
	(char *)"made",
	(char *)"make",
	(char *)"many",
	(char *)"may",
	(char *)"me",
	(char *)"meanwhile",
	(char *)"mg",
	(char *)"might",
	(char *)"ml",
	(char *)"mm",
	(char *)"mo",
	(char *)"more",
	(char *)"moreover",
	(char *)"most",
	(char *)"mostly",
	(char *)"mr",
	(char *)"much",
	(char *)"must",
	(char *)"my",
	(char *)"myself",
	(char *)"namely",
	(char *)"neither",
	(char *)"never",
	(char *)"nevertheless",
	(char *)"next",
	(char *)"no",
	(char *)"nobody",
	(char *)"noone",
	(char *)"nor",
	(char *)"not",
	(char *)"nothing",
	(char *)"now",
	(char *)"nowhere",
	(char *)"of",
	(char *)"off",
	(char *)"often",
	(char *)"on",
	(char *)"only",
	(char *)"onto",
	(char *)"or",
	(char *)"other",
	(char *)"others",
	(char *)"otherwise",
	(char *)"our",
	(char *)"ours",
	(char *)"ourselves",
	(char *)"out",
	(char *)"over",
	(char *)"own",
	(char *)"oz",
	(char *)"per",
	(char *)"perhaps",
	(char *)"pm",
	(char *)"precede",
	(char *)"presently",
	(char *)"previously",
	(char *)"pt",
	(char *)"rather",
	(char *)"regarding",
	(char *)"relate",
	(char *)"said",
	(char *)"same",
	(char *)"seem",
	(char *)"seemed",
	(char *)"seeming",
	(char *)"seems",
	(char *)"seriously",
	(char *)"several",
	(char *)"she",
	(char *)"should",
	(char *)"show",
	(char *)"showed",
	(char *)"shown",
	(char *)"since",
	(char *)"so",
	(char *)"some",
	(char *)"somehow",
	(char *)"someone",
	(char *)"something",
	(char *)"sometime",
	(char *)"sometimes",
	(char *)"somewhere",
	(char *)"still",
	(char *)"studied",
	(char *)"sub",
	(char *)"such",
	(char *)"take",
	(char *)"tell",
	(char *)"th",
	(char *)"than",
	(char *)"that",
	(char *)"the",
	(char *)"their",
	(char *)"them",
	(char *)"themselves",
	(char *)"then",
	(char *)"thence",
	(char *)"there",
	(char *)"thereafter",
	(char *)"thereby",
	(char *)"therefore",
	(char *)"therein",
	(char *)"thereupon",
	(char *)"these",
	(char *)"they",
	(char *)"this",
	(char *)"thorough",
	(char *)"those",
	(char *)"though",
	(char *)"through",
	(char *)"throughout",
	(char *)"thru",
	(char *)"thus",
	(char *)"to",
	(char *)"together",
	(char *)"too",
	(char *)"toward",
	(char *)"towards",
	(char *)"try",
	(char *)"type",
	(char *)"ug",
	(char *)"under",
	(char *)"unless",
	(char *)"until",
	(char *)"up",
	(char *)"upon",
	(char *)"us",
	(char *)"used",
	(char *)"using",
	(char *)"various",
	(char *)"very",
	(char *)"via",
	(char *)"was",
	(char *)"we",
	(char *)"were",
	(char *)"what",
	(char *)"whatever",
	(char *)"when",
	(char *)"whence",
	(char *)"whenever",
	(char *)"where",
	(char *)"whereafter",
	(char *)"whereas",
	(char *)"whereby",
	(char *)"wherein",
	(char *)"whereupon",
	(char *)"wherever",
	(char *)"whether",
	(char *)"which",
	(char *)"while",
	(char *)"whither",
	(char *)"who",
	(char *)"whoever",
	(char *)"whom",
	(char *)"whose",
	(char *)"why",
	(char *)"will",
	(char *)"with",
	(char *)"within",
	(char *)"without",
	(char *)"wk",
	(char *)"would",
	(char *)"wt",
	(char *)"yet",
	(char *)"you",
	(char *)"your",
	(char *)"yours",
	(char *)"yourself",
	(char *)"yourselves",
	(char *)"yr"
	} ;

/*
	ANT_STOP_WORD_LIST_LEN
	-----------------------
*/
long ANT_stop_word::ANT_stop_word_list_len = sizeof(ANT_stop_word::ANT_stop_word_list) / sizeof(*ANT_stop_word::ANT_stop_word_list);

