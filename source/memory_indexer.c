/*
	MEMORY_INDEXER.C
	----------------
*/
#include "memory_indexer.h"

ANT_string_pair ANT_memory_indexer::squiggle_length = ANT_string_pair("~length");
ANT_string_pair ANT_memory_indexer::squiggle_unique_term_count = ANT_string_pair("~unique_terms_in_document");
ANT_string_pair ANT_memory_indexer::squiggle_puurula_tfidf_length = ANT_string_pair("~puurula_tfidf_length");

static ANT_string_pair squiggle_puurula_tfidf_powerlaw_length_0 = ANT_string_pair("~puurula_tfidf_powerlaw_length_0");
static ANT_string_pair squiggle_puurula_tfidf_powerlaw_length_1 = ANT_string_pair("~puurula_tfidf_powerlaw_length_1");
static ANT_string_pair squiggle_puurula_tfidf_powerlaw_length_2 = ANT_string_pair("~puurula_tfidf_powerlaw_length_2");
static ANT_string_pair squiggle_puurula_tfidf_powerlaw_length_3 = ANT_string_pair("~puurula_tfidf_powerlaw_length_3");
static ANT_string_pair squiggle_puurula_tfidf_powerlaw_length_4 = ANT_string_pair("~puurula_tfidf_powerlaw_length_4");
static ANT_string_pair squiggle_puurula_tfidf_powerlaw_length_5 = ANT_string_pair("~puurula_tfidf_powerlaw_length_5");
static ANT_string_pair squiggle_puurula_tfidf_powerlaw_length_6 = ANT_string_pair("~puurula_tfidf_powerlaw_length_6");
static ANT_string_pair squiggle_puurula_tfidf_powerlaw_length_7 = ANT_string_pair("~puurula_tfidf_powerlaw_length_7");
static ANT_string_pair squiggle_puurula_tfidf_powerlaw_length_8 = ANT_string_pair("~puurula_tfidf_powerlaw_length_8");
static ANT_string_pair squiggle_puurula_tfidf_powerlaw_length_9 = ANT_string_pair("~puurula_tfidf_powerlaw_length_9");
static ANT_string_pair squiggle_puurula_tfidf_powerlaw_length_10 = ANT_string_pair("~puurula_tfidf_powerlaw_length_10");

ANT_string_pair *ANT_memory_indexer::squiggle_puurula_tfidf_powerlaw_length[] =
{
&squiggle_puurula_tfidf_powerlaw_length_0,
&squiggle_puurula_tfidf_powerlaw_length_1,
&squiggle_puurula_tfidf_powerlaw_length_2,
&squiggle_puurula_tfidf_powerlaw_length_3,
&squiggle_puurula_tfidf_powerlaw_length_4,
&squiggle_puurula_tfidf_powerlaw_length_5,
&squiggle_puurula_tfidf_powerlaw_length_6,
&squiggle_puurula_tfidf_powerlaw_length_7,
&squiggle_puurula_tfidf_powerlaw_length_8,
&squiggle_puurula_tfidf_powerlaw_length_9,
&squiggle_puurula_tfidf_powerlaw_length_10
};

static ANT_string_pair squiggle_puurula_length_0 = ANT_string_pair("~puurula_length_0");
static ANT_string_pair squiggle_puurula_length_1 = ANT_string_pair("~puurula_length_1");
static ANT_string_pair squiggle_puurula_length_2 = ANT_string_pair("~puurula_length_2");
static ANT_string_pair squiggle_puurula_length_3 = ANT_string_pair("~puurula_length_3");
static ANT_string_pair squiggle_puurula_length_4 = ANT_string_pair("~puurula_length_4");
static ANT_string_pair squiggle_puurula_length_5 = ANT_string_pair("~puurula_length_5");
static ANT_string_pair squiggle_puurula_length_6 = ANT_string_pair("~puurula_length_6");
static ANT_string_pair squiggle_puurula_length_7 = ANT_string_pair("~puurula_length_7");
static ANT_string_pair squiggle_puurula_length_8 = ANT_string_pair("~puurula_length_8");
static ANT_string_pair squiggle_puurula_length_9 = ANT_string_pair("~puurula_length_9");
static ANT_string_pair squiggle_puurula_length_10 = ANT_string_pair("~puurula_length_10");

ANT_string_pair *ANT_memory_indexer::squiggle_puurula_length[] =
{
&squiggle_puurula_length_0,
&squiggle_puurula_length_1,
&squiggle_puurula_length_2,
&squiggle_puurula_length_3,
&squiggle_puurula_length_4,
&squiggle_puurula_length_5,
&squiggle_puurula_length_6,
&squiggle_puurula_length_7,
&squiggle_puurula_length_8,
&squiggle_puurula_length_9,
&squiggle_puurula_length_10
};
