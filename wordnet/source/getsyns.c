/*
	GETSYNS.C
	---------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "/ant/source/disk.h"


/*
	Different relationships seen in WordNet 3
*/
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
CAUSE
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
"CAUSE"
} ;

struct ANT_relationship
{
long long type;
char *wordnet_identifier;
char *wordname_name;
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
{INSTANCE_HYPONYM, 	"~i", "Instance Hyponym"}
};

int number_of_relationships = sizeof(relationships) / sizeof(*relationships);


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
*/
char *get_title(char *line)
{
char *term, *term_end;
long long lex_id, current;
int times;

term = line;
term = strchr(term, ' ') + 1;
term = strchr(term, ' ') + 1;
term = strchr(term, ' ') + 1;

sscanf(term, "%02X", &times);

if (times > 1)
	printf("(");

for (current = 0; current < times; current++)
	{
	term = strchr(term, ' ') + 1;
	term_end = strchr(term, ' ');

	if (current != 0)
		printf(" ");
	printf("%*.*s", term_end - term, term_end - term, term);

	term = term_end + 1;
	lex_id = atol(term);
	}
if (times > 1)
	printf(")");

return NULL;
}

/*
	LIST_SYNSETS()
	--------------
*/
void list_synsets(char *file, char *at, long long times)
{
char type[4], pos;
long long offset;
int source_target;
ANT_relationship *relation;

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
		printf("   %s ", name_of_relationship[relation->type]);
		if (pos == 'n')
			{
			get_title(file + offset);
			puts("");
			}
		else
			printf("[%c]\n", pos);
		}
	else
		puts("ERROR - unknown relatonship");
	}
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
long long length;
char *file, **lines;
char *at, **current;

if (argc != 2)
	exit(printf("usage:%s <data.noun>\n"));

if ((file = ANT_disk::read_entire_file(argv[1])) == NULL)
	exit(printf("Cannot open file:%s\n", argv[1]));

lines = ANT_disk::buffer_to_list(file, &length);

for (current = lines; *current != NULL; current++)
	{
	long long offset;
	long long lex_filenum;
	char ss_type;
	int w_cnt, current_word;
	char word[1024];
	long long lex_id;
	long long p_cnt;


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

	get_title(*current);

	for (current_word = 0; current_word < w_cnt; current_word++)
		{
		at = strchr(at, ' ') + 1;
		at = strchr(at, ' ') + 1;
		}
	 
	p_cnt = atol(at);
	if ((at = strchr(at, ' ')) == NULL)
		continue;
	at++;

	printf(": %d synsets\n", p_cnt);
	list_synsets(file, at, p_cnt);
	}

delete [] lines;
delete [] file;

return 0;
}