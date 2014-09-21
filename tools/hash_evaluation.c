#include <stdio.h>
#include <stdlib.h>
#include "hash_header.h"
#include "hash_header_collapse.h"
#include "hash_random.h"
#include "hash_matt.h"
#include "hash_superfast.h"
#include "string_pair.h"
#include "disk.h"

typedef unsigned long (*hash_function)(ANT_string_pair *);
typedef unsigned long (*uniqable)(unsigned long);

struct {
	char *n;
	hash_function f;
	uniqable u;
} functions[] = {
	{"matt", ANT_hash_matt_24, ANT_hash_matt_uniq},
	//{"superfast", ANT_hash_superfast_24, NULL},
	//{"random", ANT_hash_random_24, NULL},
};

/*
	ASCIIFY()
	---------
	Sets the is ascii flag on a string, which isn't really an ascii
	flag, more of a "should be considered for a unique hash" flag
*/
void asciify(ANT_string_pair *s)
{
int i;
int ascii = TRUE;

for (i = 0; i < s->length() && ascii; i++)
	{
	ascii = ascii && (((*s)[i] & 0x80) == 0);
	ascii = ascii && !ANT_isupper((*s)[i]);
	}

s->is_ascii = ascii;
}

/*
	MAIN()
	------
	Decorates a dictionary file by prepending columns with hash values for different hash functions
*/
int main(int argc, char **argv)
{
if (argc != 2)
	exit(printf("Usage: %s <filename>\n", argv[0]));

long long number_terms;
char **lines = ANT_disk::buffer_to_list(ANT_disk::read_entire_file(argv[1]), &number_terms);
char *space;
ANT_string_pair *term = NULL;

int function;
size_t num_functions = sizeof(functions) / sizeof(*functions);
unsigned long value;

for (function = 0; function < num_functions; function++)
	printf("%s ", functions[function].n);
printf("term cf df\n"); // always include the term, cf and df

for (int line = 0; line < number_terms; line++)
	{
	delete term;

	space = strchr(lines[line], ' ');
	term = new ANT_string_pair(lines[line], space - lines[line]);
	asciify(term);

	for (function = 0; function < num_functions; function++)
		{
		value = functions[function].f(term);
		if (functions[function].u != NULL && functions[function].u(value)) 
			printf("-1 ");
		else
			printf("%lu ", functions[function].f(term));
		}
	printf("%s\n", lines[line]);
	}

return EXIT_SUCCESS;
}
