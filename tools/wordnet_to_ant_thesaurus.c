/*
	WORDNET_TO_ANT_THESAURUS.C
	--------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../source/ctypes.h"
#include "../source/disk.h"
#include "../source/file.h"
#include "../source/fundamental_types.h"
#include "../source/thesaurus_wordnet.h"
#include "../source/thesaurus_relationship.h"

uint32_t ANT_ID_THESAURUS_SIGNATURE_MAJOR = 0x54505341;	//"ASPT" (Intel Byte Order)
uint32_t ANT_ID_THESAURUS_SIGNATURE_MINOR = 0x00000003;	// version 0.3		BCD Major / Minor version number
uint64_t ANT_ID_THESAURUS_WORDNET = 0x54454e44524f5700;	//"WORDNET"(Intel Byte Order)

struct ANT_relationship
{
long type;
char *wordnet_identifier;
char *wordnet_name;
} ;

enum {
	MEMBER_OF_TOPIC,
	MEMBER_OF_REGION,
	MEMBER_OF_USAGE,
	HYPONYM,
	INSTANCE_HYPONYM,
	ANTONYM,
	MEMBER_HOLONYM,
	PART_HOLONYM,
	SUBSTANCE_HOLONYM,
	VERB_GROUP,
	MEMBER_MERONYM,
	PART_MERONYM,
	SUBSTANCE_MERONYM,
	SIMILAR_TO,
	ENTAILMENT,
	DOMAIN_OF_TOPIC,
	DOMAIN_OF_REGION,
	DOMAIN_OF_USAGE,
	HYPERNYM,
	INSTANCE_HYPERNYM,
	FROM_ABJECTIVE,
	PERTAINYM,
	SEE_ALSO,
	DERIVED_FORM,
	PRINCIPLE_OF_VERB,
	ATTRIBUTE,
	CAUSE,
	SYNONYM,
	SENTINAL					// not a relationship, used to mark the end of lists
	};

long ANT_relationship_of_wordnet_relationship[] = 
{
0,
0,
0,
ANT_thesaurus_relationship::HYPONYM,
ANT_thesaurus_relationship::HYPONYM,
ANT_thesaurus_relationship::ANTONYM,
ANT_thesaurus_relationship::HOLONYM,
ANT_thesaurus_relationship::HOLONYM,
ANT_thesaurus_relationship::HOLONYM,
0,
ANT_thesaurus_relationship::MERONYM,
ANT_thesaurus_relationship::MERONYM,
ANT_thesaurus_relationship::MERONYM,
0,
0,
0,
0,
0,
ANT_thesaurus_relationship::HYPERNYM,
ANT_thesaurus_relationship::HYPERNYM,
0,
0,
0,
0,
0,
0,
0,
ANT_thesaurus_relationship::SYNONYM
};

char *name_of_relationship[] = 
{
"MEMBER_OF_TOPIC",
"MEMBER_OF_REGION",
"MEMBER_OF_USAGE",
"HYPONYM",
"INSTANCE_HYPONYM",
"ANTONYM",
"MEMBER_HOLONYM",
"PART_HOLONYM",
"SUBSTANCE_HOLONYM",
"VERB_GROUP",
"MEMBER_MERONYM",
"PART_MERONYM",
"SUBSTANCE_MERONYM",
"SIMILAR_TO",
"ENTAILMENT",
"DOMAIN_OF_TOPIC",
"DOMAIN_OF_REGION",
"DOMAIN_OF_USAGE",
"HYPERNYM",
"INSTANCE_HYPERNYM",
"FROM_ABJECTIVE",
"PERTAINYM",
"SEE_ALSO",
"DERIVED_FORM",
"PRINCIPLE_OF_VERB",
"ATTRIBUTE",
"CAUSE",
"SYNONYM",
"SENTINAL"
} ;

ANT_relationship relationships[] =
{
{ANTONYM, 				"!", "Antonym"},
{MEMBER_HOLONYM, 		"#m", "Member holonym"},
{PART_HOLONYM, 		"#p", "Part holonym"},
{SUBSTANCE_HOLONYM, 	"#s", "Substance holonym"},
{VERB_GROUP, 			"$", "Verb Group"},
{MEMBER_MERONYM, 		"%m", "Member meronym"},
{PART_MERONYM, 		"%p", "Part meronym"},
{SUBSTANCE_MERONYM, 	"%s", "Substance meronym"},
{SIMILAR_TO, 			"&", "Similar to"},
{ENTAILMENT, 			"*", "Entailment"},
{DERIVED_FORM,			"+", "Derivationally related form"},
{MEMBER_OF_TOPIC,  	"-c", "Member of this domain - TOPIC"},
{MEMBER_OF_REGION, 	"-r", "Member of this domain - REGION"},
{MEMBER_OF_USAGE,  	"-u", "Member of this domain - USAGE"},
{DOMAIN_OF_TOPIC,		";c", "Domain of synset - TOPIC"},
{DOMAIN_OF_REGION,		";r", "Domain of synset - REGION"},
{DOMAIN_OF_USAGE,		";u", "Domain of synset - USAGE"},
{PRINCIPLE_OF_VERB,	"<", "Participle of verb"},
{ATTRIBUTE,			"=", "Attribute"},
{CAUSE,				">", "Cause"},
{HYPERNYM,				"@", "Hypernym"},
{INSTANCE_HYPERNYM,	"@i", "Instance Hypernym"},
//{FROM_ABJECTIVE,		"\", "Derived from adjective"},
{PERTAINYM,			"\\", "Pertainym (pertains to noun)"},
{SEE_ALSO,				"^", "Also see"},
{HYPONYM,   			"~", "Hyponym"},
{INSTANCE_HYPONYM, 	"~i", "Instance Hyponym"},
{SYNONYM,				"~z", "Synonym"}				// NOT A WORDNET TYPE
};

int number_of_relationships = sizeof(relationships) / sizeof(*relationships);

/*
	class ANT_WORD
	--------------
*/
class ANT_word
{
public:
	char *word;

public:
	ANT_word(char *word, size_t length)
		{
		char *into, *outof;
		long as_string = false;

		this->word = new char [length + 3];
		this->word[0] = '"';

		for (into = this->word + 1, outof = word; (size_t)(outof - word) < length; outof++)
			{
			if (*outof == '(')
				{
				*into = '\0';
				break;
				}
			else if (ANT_isalnum(*outof))
				*into++ = *outof;
			else
				{
				as_string = true;
				*into++ = ' ';
				}
			}

		/*
			remove white space from the end of the string (can happen when "u.s.a." -> "u s a "
		*/
		into--;
		while (ANT_isspace(*into))
			*into--;

		into++;

		if (as_string)
			*into++ = '"';
		else
			this->word++;

		*into = '\0';

		strlower(word);		// convert the whold string to lower case
		}
	~ANT_word() {}			// { delete [] word; }
};

/*
	class ANT_WORD_TYPE
	-------------------
*/
class ANT_word_type
{
public:
	ANT_word **words;
	long relationship;

public:
	ANT_word_type(ANT_word **words, long relationship) { this->words = words; this->relationship = relationship; }
	~ANT_word_type() { delete [] words; }
};

/*
	class ANT_WORDNET_ROW
	---------------------
*/
class ANT_wordnet_row
{
public:
	ANT_word **synonyms;
	ANT_word_type **synsets;

public:
	ANT_wordnet_row(ANT_word **synonyms, ANT_word_type **synsets) { this->synonyms = synonyms; this->synsets = synsets; }
	~ANT_wordnet_row() { delete [] synonyms; delete [] synsets; }

	void render(void);
};

/*
	class ANT_WORDNET_HEADWORD
	--------------------------
*/
class ANT_wordnet_headword
{
public:
	ANT_word *word;
	ANT_wordnet_row *row;
	unsigned long long file_start;
	unsigned long long file_length;

public:
	static int cmp(const void *a, const void *b) { return strcmp(((ANT_wordnet_headword *)a)->word->word, ((ANT_wordnet_headword *)b)->word->word); }
};

static const size_t TERMS_IN_WORDNET = 1024 * 1024;
ANT_wordnet_headword all_of_wordnet[TERMS_IN_WORDNET];
ANT_wordnet_headword *current_wordnet_term = all_of_wordnet;

/*
	ANT_WORDNET_ROW::RENDER()
	-------------------------
*/
void ANT_wordnet_row::render(void)
{
ANT_word **current_word;
ANT_word_type **relationship;

for (current_word = synonyms; *current_word != NULL; current_word++)
	printf("%s\n", (*current_word)->word);

for (relationship = synsets; *relationship != NULL; relationship++)
	{
	printf("   %s\n", name_of_relationship[(*relationship)->relationship]);
	for (current_word = (*relationship)->words; *current_word != NULL; current_word++)
		printf("      %s\n", (*current_word)->word);
	}
}

/*
	class ANT_TERM_RELATIONSHIP
	---------------------------
*/
class ANT_term_relationship
{
public:
	char *term;
	long relationship;

public:
	static int cmp(const void *a, const void *b) { return strcmp(((ANT_term_relationship *)a)->term, ((ANT_term_relationship *)b)->term); }
} ;

/*
	RELATIONSHIP_CMP()
	------------------
*/
int relationship_cmp(const void *a, const void *b)
{
char *string = (char *)a;
ANT_relationship *relation = (ANT_relationship *)b;

return strcmp(string, relation->wordnet_identifier);
}

/*
	GET_TITLE()
	-----------
	returns a NULL terminated list
*/
ANT_word **get_title(char *line)
{
ANT_word **results_list;
char *term, *term_end;
long long lex_id, current;
int times;

term = line;
term = strchr(term, ' ') + 1;
term = strchr(term, ' ') + 1;
term = strchr(term, ' ') + 1;

sscanf(term, "%02X", &times);
//printf("[%d]", times);

results_list = new ANT_word *[times + 1];
for (current = 0; current < times; current++)
	{
	term = strchr(term, ' ') + 1;
	term_end = strchr(term, ' ');

	results_list[current] = new ANT_word(term, term_end - term);

	term = term_end + 1;
	lex_id = atol(term);
	}

results_list[times] = NULL;

return results_list;
}

/*
	LIST_SYNSETS()
	--------------
*/
ANT_word_type **list_synsets(char *noun_file, char *verb_file, char *adjective_file, char *adverb_file, char *at, long long times)
{
char type[4], pos;
long long offset;
int source_target;
ANT_relationship *relation;
ANT_word **synonym_set;
ANT_word_type **relationship_set, **current_relationship;
char *file;

current_relationship = relationship_set = new ANT_word_type *[(size_t)(times + 1)];
while (times-- > 0)
	{
	memset(type, 0, sizeof(type));
	type[0] = *at++;
	if (*at != ' ')
		type[1] = *at++;

	if ((at = strchr(at, ' ')) == NULL)
		continue;
	at++;

	offset = atol(at);
	if ((at = strchr(at, ' ')) == NULL)
		continue;
	at++;

	pos = *at++;
	at++;

	sscanf(at, "%04x", &source_target);
	if ((at = strchr(at, ' ')) == NULL)
		continue;
	at++;

	if ((relation = (ANT_relationship *)bsearch(type, relationships, number_of_relationships, sizeof(*relationships), relationship_cmp)) != NULL)
		{
		if (ANT_relationship_of_wordnet_relationship[relation->type] == 0)
			continue;		// skip stuff ANT knowns nothing about

		if (pos == 'n')
			file = noun_file;
		else if (pos == 'v')
			file = verb_file;
		else if (pos == 'a')
			file = adjective_file;
		else if (pos == 'r')
			file = adverb_file;
		else
			file = NULL;

		if (file != NULL)
			{
			synonym_set = get_title(file + offset);
			*current_relationship = new ANT_word_type(synonym_set, relation->type);
			}
		else
			exit(printf("[%c]\n", pos));
		}
	else
		exit(printf("ERROR - unknown relatonship\n"));

	current_relationship++;
	}

*current_relationship = NULL;

return relationship_set;
}

/*
	PROCESS_ONE_FILE()
	------------------
*/
void process_one_file(char **lines, char *noun_file, char *verb_file, char *adjective_file, char *adverb_file)
{
char *at, **current;
long long offset;
long long lex_filenum;
char ss_type;
int w_cnt, current_word;
long long p_cnt;

for (current = lines; *current != NULL; current++)
	{
	at = *current;

	if (*at == ' ')
		continue;

	offset = atol(at);
	if ((at = strchr(at, ' ')) == NULL)
		continue;
	at++;
	
	lex_filenum = atol(at);
	if ((at = strchr(at, ' ')) == NULL)
		continue;
	at++;
	
	ss_type = *at;
	at += 2;

	sscanf(at, "%02x", &w_cnt);
	if ((at = strchr(at, ' ')) == NULL)
		continue;
	at++;

	for (current_word = 0; current_word < w_cnt; current_word++)
		{
		at = strchr(at, ' ') + 1;
		at = strchr(at, ' ') + 1;
		}

	ANT_word **title_set = get_title(*current);

	p_cnt = atol(at);
	if ((at = strchr(at, ' ')) == NULL)
		continue;
	at++;

	ANT_word_type **synsets = list_synsets(noun_file, verb_file, adjective_file, adverb_file, at, p_cnt);
	ANT_wordnet_row *entry = new ANT_wordnet_row(title_set, synsets);
	ANT_word **current_word;

	for (current_word = entry->synonyms; *current_word != NULL; current_word++)
		{
		current_wordnet_term->word = *current_word;
		current_wordnet_term->row = entry;
		current_wordnet_term->file_start = 0;
		current_wordnet_term->file_length = 0;

		current_wordnet_term++;
		if (current_wordnet_term >= all_of_wordnet + TERMS_IN_WORDNET)
			exit(printf("expand the size of all_of_wordnet[] by making  TERMS_IN_WORDNET larger\n"));
		}
	}

current_wordnet_term->word = NULL;		// NULL terminate the list
}

/*
	WRITE_NORMALISED_STRING()
	-------------------------
*/
void write_normalised_string(ANT_file *outfile, char *string)
{
outfile->write(string, strlen(string) + 1);
}

/*
	BUILD_INDEX()
	-------------
*/
void build_index(void)
{
long combined_relationship;
ANT_term_relationship *disk_leaf, *pos_in_leaf, *dump;
char file_header[] = "ANT Search Engine Thesaurus File\n\0\0";
char *previous_headword, *previous_tailword;
ANT_wordnet_headword *current;
ANT_wordnet_row *current_row;
ANT_word **term, **current_word;
ANT_word_type **relationship;
ANT_file *outfile;
uint8_t one_byte;
uint64_t eight_byte, start_of_header, terms_in_header, length_of_longest_synset, length_of_current_synset, bytes_in_longest_leaf;

disk_leaf = new ANT_term_relationship[1024 * 1024];		// 1 million is easily enough
outfile = new ANT_file;
outfile->open("wordnet.aspt", "wb");
outfile->write(file_header, sizeof(file_header));

previous_headword = "";
length_of_longest_synset = 0;
length_of_current_synset = 0;
pos_in_leaf = disk_leaf;
for (current = all_of_wordnet; current < current_wordnet_term; current++)
	{
	current->file_start = outfile->tell();

	if (strcmp(previous_headword, current->word->word) != 0)
		{
		length_of_current_synset = 0;
		pos_in_leaf = disk_leaf;
		}

	current_row = current->row;
	for (term = current_row->synonyms; *term != NULL; term++)
		if (strcmp(current->word->word, (*term)->word) != 0)				// a term is nothing to itself
			{
//			printf("  SYNONYM %s\n", (*term)->word);
			pos_in_leaf->relationship = SYNONYM;
			pos_in_leaf->term = (*term)->word;
			length_of_current_synset++;
			pos_in_leaf++;
			}

	for (relationship = current_row->synsets; *relationship != NULL; relationship++)
		for (current_word = (*relationship)->words; *current_word != NULL; current_word++)
			if (strcmp(current->word->word, (*current_word)->word) != 0)	// a term is nothing to itself
				{
	//			printf("  %s %s\n", name_of_relationship[(*relationship)->relationship], (*current_word)->word);
				pos_in_leaf->relationship = (*relationship)->relationship;
				pos_in_leaf->term = (*current_word)->word;
				length_of_current_synset++;
				pos_in_leaf++;
				}

	if (length_of_current_synset > length_of_longest_synset)
		length_of_longest_synset = length_of_current_synset;

	/*
		If we're the last term or (not the same as the next term) then we write to disk
	*/
	if ((current + 1)->word == NULL || strcmp((current + 1)->word->word, current->word->word) != 0)
		{
		qsort(disk_leaf, pos_in_leaf - disk_leaf, sizeof(*disk_leaf), ANT_term_relationship::cmp);

		dump = disk_leaf;
		while (dump < pos_in_leaf)
			{
			combined_relationship = ANT_relationship_of_wordnet_relationship[dump->relationship];
			previous_tailword = dump->term;
			for (dump++; dump < pos_in_leaf && strcmp(previous_tailword, dump->term) == 0; dump++)
				combined_relationship |= ANT_relationship_of_wordnet_relationship[dump->relationship];

			if (combined_relationship != 0)
				{
				one_byte = (uint8_t)combined_relationship;
				outfile->write(&one_byte, sizeof(one_byte));
				write_normalised_string(outfile, previous_tailword);
				}
			}
		}

	current->file_length = outfile->tell() - current->file_start;

//	printf("%s %lld %lld\n", current->word->word, current->file_start, current->file_length);
	previous_headword = current->word->word;
	}

start_of_header = outfile->tell();
bytes_in_longest_leaf = 0;
terms_in_header = 0;
previous_headword = "";
for (current = all_of_wordnet; current < current_wordnet_term; current++)
	{
	if ((current + 1)->word == NULL || strcmp((current + 1)->word->word, current->word->word) != 0)
		{
		if (current->file_length != 0)
			{
			terms_in_header++;
		//	printf("%s\n", current->word->word);
			write_normalised_string(outfile, current->word->word);

			eight_byte = (uint64_t)current->file_start;
			outfile->write((char *)&eight_byte, sizeof(eight_byte));

			eight_byte = (uint64_t)current->file_length;
			outfile->write((char *)&eight_byte, sizeof(eight_byte));

			if (current->file_length > bytes_in_longest_leaf)
				bytes_in_longest_leaf = current->file_length;
			}
//		else
//			printf("Dropping term:%s\n", current->word->word);
		}
	}

outfile->write((char *)&start_of_header, sizeof(start_of_header));
outfile->write((char *)&terms_in_header, sizeof(terms_in_header));
outfile->write((char *)&length_of_longest_synset, sizeof(length_of_longest_synset));
outfile->write((char *)&bytes_in_longest_leaf, sizeof(bytes_in_longest_leaf));

outfile->write((char *)&ANT_ID_THESAURUS_WORDNET, sizeof(ANT_ID_THESAURUS_WORDNET));
outfile->write((char *)&ANT_ID_THESAURUS_SIGNATURE_MINOR, sizeof(ANT_ID_THESAURUS_SIGNATURE_MINOR));
outfile->write((char *)&ANT_ID_THESAURUS_SIGNATURE_MAJOR, sizeof(ANT_ID_THESAURUS_SIGNATURE_MAJOR));

outfile->close();
delete outfile;
delete [] disk_leaf;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
long long length;
char *noun_file, **noun_lines;
char *verb_file, **verb_lines;
char *adjective_file, **adjective_lines;
char *adverb_file, **adverb_lines;

if (argc != 5)
	exit(printf("usage:%s <data.noun> <data.verb> <data.adj> <data.adv>\n", argv[0]));

if ((noun_file = ANT_disk::read_entire_file(argv[1])) == NULL)
	exit(printf("Cannot open file:%s\n", argv[1]));

if ((verb_file = ANT_disk::read_entire_file(argv[2])) == NULL)
	exit(printf("Cannot open file:%s\n", argv[2]));

if ((adjective_file = ANT_disk::read_entire_file(argv[3])) == NULL)
	exit(printf("Cannot open file:%s\n", argv[3]));

if ((adverb_file = ANT_disk::read_entire_file(argv[4])) == NULL)
	exit(printf("Cannot open file:%s\n", argv[4]));

noun_lines = ANT_disk::buffer_to_list(noun_file, &length);
verb_lines = ANT_disk::buffer_to_list(verb_file, &length);
adjective_lines = ANT_disk::buffer_to_list(adjective_file, &length);
adverb_lines = ANT_disk::buffer_to_list(adverb_file, &length);

process_one_file(noun_lines, noun_file, verb_file, adjective_file, adverb_file);
process_one_file(verb_lines, noun_file, verb_file, adjective_file, adverb_file);
process_one_file(adjective_lines, noun_file, verb_file, adjective_file, adverb_file);
process_one_file(adverb_lines, noun_file, verb_file, adjective_file, adverb_file);

qsort(all_of_wordnet, current_wordnet_term - all_of_wordnet, sizeof(*all_of_wordnet), ANT_wordnet_headword::cmp);

build_index();

delete [] noun_lines;
delete [] verb_lines;
delete [] adjective_lines;
delete [] adverb_lines;

delete [] noun_file;
delete [] verb_file;
delete [] adjective_file;
delete [] adverb_file;

return 0;
}