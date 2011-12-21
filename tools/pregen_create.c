/*
	PREGEN_CREATE.C
	---------------
*/
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <sstream>
#include <utility>
#include <algorithm>
#include <cmath>

#ifdef _MSC_VER
	#include <windows.h>
#else
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <sys/mman.h>
#endif

#include "str.h"
#include "string_pair.h"
#include "file.h"
#include "disk.h"
#include "pregen.h"
#include "search_engine_accumulator.h"
#include "../atire/indexer_param_block_pregen.h"
#include "pregens_writer.h"

/*
	MAP_ENTIRE_FILE()
	-----------------
*/
#ifdef _MSC_VER
	char *map_entire_file(const char *filename, long long *filesize)
	{
	HANDLE mapping;
	HANDLE fp;
	char *result;

	(void) filesize; //TODO fill this parameter

	fp = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fp == INVALID_HANDLE_VALUE)
		return NULL;

	mapping = CreateFileMapping(fp, NULL, PAGE_READONLY, 0, 0, NULL);

	if (mapping == NULL)
		return NULL;

	result = (char*) MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0);

	return result;
	}
#else
	char *map_entire_file(const char *filename, long long *filesize)
	{
	int fd = open(filename, O_RDONLY, (mode_t)0600);
	struct stat buffer;
	char *result;

	(void) filesize; //TODO fill this parameter

	if (fd == -1)
		return NULL;

	fstat(fd, &buffer);

	result = (char*) mmap (0, buffer.st_size, PROT_READ, MAP_SHARED, fd, 0);

	if (result==MAP_FAILED)
		return NULL;

	return result;
	}
#endif

/*
	MAIN()
	------
*/
int main(int argc, char ** argv)
{
ANT_indexer_param_block_pregen pregen_params;
int num_pregens;
char **pregen_type, **pregen_filenames;

assert(argc >= 3);

num_pregens = (argc - 2) / 2;
pregen_type = new char*[num_pregens];
pregen_filenames = new char*[num_pregens];

printf("Pregen field size is %d bytes\n\n", sizeof(ANT_pregen_t));

for (int i = 2, pregen_index = 0; i < argc; i += 2, pregen_index++)
	{
	char * field_name = argv[i];
	char * field_type = argv[i + 1];
	std::ostringstream filenamebuf;

	pregen_type[pregen_index] = field_type;

	filenamebuf << "pregen." << field_name << "." << field_type;

	pregen_filenames[pregen_index] = strnew(filenamebuf.str().c_str());

	if (!pregen_params.add_pregen_field(field_name, field_type))
		exit(printf("Unknown pregen field type '%s'\n", field_type));
	}

ANT_pregens_writer pregen_writer;
char *doclist, *cur;

fprintf(stderr, "Generating pregens from doclist...\n");

//Read document names from the .doclist file
doclist = map_entire_file(argv[1], NULL);

if (!doclist)
	{
	fprintf(stderr, "Couldn't read doclist\n");
	exit(-1);
	}

//Create pregen field writers for each of the fields we're examining...
for (int i = 0; i < pregen_params.num_pregen_fields; i++)
	if (!pregen_writer.add_field(pregen_filenames[i], pregen_params.pregens[i].field_name, pregen_params.pregens[i].type))
		exit(fprintf(stderr, "Couldn't open pregen file '%s'\n", pregen_filenames[i]));

long long docindex = 0;

cur = doclist;
while (*cur)
	{
	char * docnameend = strchr(cur, '\n');

	if (!docnameend)
		break;

	pregen_writer.process_document(docindex, ANT_string_pair(cur, docnameend - cur));

	cur = docnameend + 1;
	docindex++;
	}

pregen_writer.close();

//Leak everything
return EXIT_SUCCESS;
}
