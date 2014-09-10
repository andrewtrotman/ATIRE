#include <stdio.h>
#include <stdlib.h>
#include "hash_header.h"
#include "hash_header_collapse.h"
#include "hash_random.h"
#include "hash_superfast.h"
#include "string_pair.h"
#include "disk.h"

typedef unsigned long (*hash_function)(ANT_string_pair *);

struct {
	hash_function f;
	char *n;
} functions[] = {
	{ANT_hash_random_24, "random"},
	{ANT_hash_header_24, "header_num"},
};

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

for (function = 0; function < num_functions; function++) {
	printf("%s ", functions[function].n);
}
printf("term cf df\n"); // always include the term, cf and df

for (int line = 0; line < number_terms; line++)
	{
	delete term;

	space = strchr(lines[line], ' ');
	term = new ANT_string_pair(lines[line], space - lines[line]);

	for (function = 0; function < num_functions; function++)
		printf("%lu ", functions[function].f(term));
	printf("%s\n", lines[line]);
	}

return EXIT_SUCCESS;
}
