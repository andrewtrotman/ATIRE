/*
 * language.cpp
 *
 *  Created on: Sep 28, 2009
 *      Author: monfee
 */

#include "language.h"
#include <search.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <string>

using namespace std;

const char *language::EN_STOP_WORDS[] = {
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
//{"a", "about", "above", "across", "after",
//										"afterwards", "again", "against", "all", "almost",
//										"alone", "already", "also","although", "always",
//										"am","among", "amongst", "amoungst", "amount",  "an",
//										"and", "another", "any","anyhow","anyone","anything",
//										"anyway", "anywhere", "are", "around", "as", "at",
//										"back","be","became", "because","become","becomes",
//										"becoming", "been", "before", "beforehand", "behind",
//										"being", "below", "beside", "besides", "between",
//										"beyond", "bill", "both", "bottom","but", "by", "call",
//										"can", "cannot", "cant", "co", "con", "could", "couldnt",
//										"cry", "de", "describe", "detail", "do", "done", "down",
//										"due", "during", "each", "eg", "eight", "either",
//										"eleven","else", "elsewhere", "empty", "enough", "etc",
//										"even", "ever", "every", "everyone", "everything",
//										"everywhere", "except", "few", "fifteen", "fify", "fill",
//										"find", "fire", "first", "five", "for", "former", "formerly",
//										"forty", "found", "four", "from", "front", "full", "further",
//										"get", "give", "go", "had", "has", "hasnt", "have", "he",
//										"hence", "her", "here", "hereafter", "hereby", "herein",
//										"hereupon", "hers", "herself", "him", "himself", "his",
//										"how", "however", "hundred", "ie", "if", "in", "inc",
//										"indeed", "interest", "into", "is", "it", "its", "itself",
//										"keep", "last", "latter", "latterly", "least", "less", "ltd",
//										"made", "many", "may", "me", "meanwhile", "might", "mill",
//										"mine", "more", "moreover", "most", "mostly", "move", "much",
//										"must", "my", "myself", "name", "namely", "neither", "never",
//										"nevertheless", "next", "nine", "no", "nobody", "none",
//										"noone", "nor", "not", "nothing", "now", "nowhere", "of",
//										"off", "often", "on", "once", "one", "only", "onto", "or",
//										"other", "others", "otherwise", "our", "ours", "ourselves",
//										"out", "over", "own","part", "per", "perhaps", "please", "put",
//										"rather", "re", "same", "see", "seem", "seemed", "seeming",
//										"seems", "serious", "several", "she", "should", "show", "side",
//										"since", "sincere", "six", "sixty", "so", "some", "somehow",
//										"someone", "something", "sometime", "sometimes", "somewhere",
//										"still", "such", "system", "take", "ten", "than", "that", "the",
//										"their", "them", "themselves", "then", "thence", "there",
//										"thereafter", "thereby", "therefore", "therein", "thereupon",
//										"these", "they", "thickv", "thin", "third", "this", "those",
//										"though", "three", "through", "throughout", "thru", "thus",
//										"to", "together", "too", "top", "toward", "towards", "twelve",
//										"twenty", "two", "un", "under", "until", "up", "upon", "us",
//										"very", "via", "was", "we", "well", "were", "what", "whatever",
//										"when", "whence", "whenever", "where", "whereafter", "whereas",
//										"whereby", "wherein", "whereupon", "wherever", "whether", "which",
//										"while", "whither", "who", "whoever", "whole", "whom", "whose",
//										"why", "will", "with", "within", "without", "would", "yet",
//										"you", "your", "yours", "yourself", "yourselves"};

int language::EN_STOP_WORDS_LENGTH = sizeof(EN_STOP_WORDS) /sizeof(char *);

std::set<std::string> language::STOP_WORD_SET(EN_STOP_WORDS, EN_STOP_WORDS + EN_STOP_WORDS_LENGTH);

language::language()
{

}

language::~language()
{

}

int language::string_compare(const void *a, const void *b)
{
	const char *l = (const char *)a;
	const char *r = (const char *)b;
	int ret = strcmp(l, r);

	if (ret == 0)
		printf("I found it");
	return ret;
}

bool language::isstopword(const char *term)
{
//	size_t nelm = EN_STOP_WORDS_LENGTH;
//	const char **base = EN_STOP_WORDS;
//	size_t size = sizeof(char *);
//
////	char *found = NULL;
////	found = (char *)lfind(term, EN_STOP_WORDS, &nelm, size, string_compare);
//	//return lfind(term, *EN_STOP_WORDS, static_cast<size_t *>(&EN_STOP_WORDS_LENGTH), sizeof(char *), strcmp);
//	for (int i = 0; i < nelm; i++) {
//		if (strcmp(*base, term) == 0)
//			return true;
//		base++;
//	}
//	return false;
	//return found != NULL;
//	if (bsearch(&term, EN_STOP_WORDS, EN_STOP_WORDS_LENGTH, sizeof(*EN_STOP_WORDS), (int(*)(const void*,const void*)) strcmp) == NULL)
//		return 0;
//
//	return 1;
	return STOP_WORD_SET.find(term) != STOP_WORD_SET.end();
}

void language::remove_en_stopword(std::string& source, const char *sep)
{
	if (source.length() == 0)
		return;

	string::size_type pos = 0;
	string::size_type pos1 = 0;
	while (source[pos] == ' ')
		++pos;

	pos1 = pos;
	while ((pos = source.find(" ", pos)) != string::npos) {
		if (isstopword(string(source, pos1, pos - pos1).c_str())) {
			source.replace(pos1, pos - pos1 + 1, sep);
			pos = pos1 + 1;
		}

		while (pos < source.length() && source[pos] == ' ')
			++pos;
		pos1 = pos;
	}
}

void language::collect_chinese_from_xml_entities(std::string& text, const char *source)
{
	//char *end = source + strlen(source);
	const char *e_start = NULL, *e_end = NULL; // entity start
	long cp = 0;
	bool is_hex = false;
	char bytes[5];

	e_start = source;
	while ((e_start = strstr(e_start, "&#")) != NULL/* && e_start < end*/) {
		if (e_end != NULL && (e_end + 1) != e_start)
			text.append(" ");

		e_start += 2;
		e_end = strchr(e_start, ';');
		if (*e_start == 'x')
			cp = strtol(e_start, NULL, 16);
		else
			cp = strtol(e_start, NULL, 10);
		codepoint_to_utf8(bytes, cp);
		if (is_cjk_language(bytes))
			text.append(bytes);
//		else
//			e_end = NULL;
	}

	collect_chinese(text, source);
}

void language::collect_chinese(std::string& text, const char *source, bool segmentation)
{
	const char *e_start = NULL, *e_end = NULL; // entity start
	// collect the Chinese, if they are not showed as entities
	e_start = source;
	const char *end = source + strlen(source);
	while (e_start != '\0' && e_start < end) {
		if (*e_start & 0x80 && isutf8((char *)e_start)) {
//			if (*e_start == '\351')
//				puts("I got you");
			if (is_cjk_language((char *)e_start)) {
				int number_of_bytes = utf8_bytes((char *)e_start);
				std::string ch(e_start, e_start + number_of_bytes);
				text.append(ch);
				e_start += number_of_bytes;
				if (!is_cjk_language((char *)e_start))
					text.append(" ");
				else
					if (segmentation)
						text.append(" ");
			}
			else
				e_start += utf8_bytes((char *)e_start);
		}
		else
			++e_start;
	}
}

void language::collect_chinese_from_xml_entities(term_freq_map_type& term_map, const char *source)
{
	//char *end = source + strlen(source);
	const char *e_start = NULL, *e_end = NULL; // entity start
	long cp = 0;
	bool is_hex = false;
	char bytes[5];
	string text;
	term_freq_map_type::iterator it;

	e_start = source;
	while ((e_start = strstr(e_start, "&#")) != NULL/* && e_start < end*/) {
		if (e_end != NULL && (e_end + 1) != e_start) {
			if ((it = term_map.find(text)) != term_map.end())
				it->second++;
			else
				term_map.insert(make_pair(text, 1));
			text.clear();
			//text.append(" ");
		}

		e_start += 2;
		e_end = strchr(e_start, ';');
		if (*e_start == 'x')
			cp = strtol(e_start, NULL, 16);
		else
			cp = strtol(e_start, NULL, 10);
		codepoint_to_utf8(bytes, cp);
		if (is_cjk_language(bytes))
			text.append(bytes);
//		else
//			e_end = NULL;
	}

	collect_chinese(text, source);
}

void language::collect_chinese(term_freq_map_type& term_map, const char *source)
{
	const char *e_start = NULL, *e_end = NULL; // entity start
	string text;
	term_freq_map_type::iterator it;

	// collect the Chinese, if they are not showed as entities
	e_start = source;
	const char *end = source + strlen(source);
	while (e_start != '\0' && e_start < end) {
		if (*e_start & 0x80 && isutf8((char *)e_start)) {
//			if (*e_start == '\351')
//				puts("I got you");
			if (is_cjk_language((char *)e_start)) {
				int number_of_bytes = utf8_bytes((char *)e_start);
				std::string ch(e_start, e_start + number_of_bytes);
				text.append(ch);
				e_start += number_of_bytes;
				if (!is_cjk_language((char *)e_start)) {
					if ((it = term_map.find(text)) != term_map.end())
						it->second++;
					else
						term_map.insert(make_pair(text, 1));
					text.clear();
				}
			}
			else
				e_start += utf8_bytes((char *)e_start);
		}
		else
			++e_start;
	}
}

void language::remove_xml_entities(std::string& source)
{
	if (source.length() == 0)
		return;

	string::size_type pos = 0;
	string::size_type pos1 = 0;

	while ((pos1 = source.find("&", pos1)) != string::npos) {
		pos = pos1;
		pos++;

		while (isalnum(source[pos]) || (source[pos]) == ';' || (source[pos]) == '#')
			++pos;

		source.replace(pos1, pos - pos1, " ");
	}
}

int language::count_terms(const char *src)
{
	int count = 0;
	const char *current = src;
	while (*current != '\0') {
		while (*current != '\0' && !isalpha(*current))
			++current;

		if (*current == '\0')
			break;
		++count;
		while (*current != '\0' && isalpha(*current))
			++current;
	}
	return count;
}
