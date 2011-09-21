/**
 * Generate a character histogram from a specified field of a doclist file.
 */

#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <cassert>

#ifdef _MSC_VER
	#include <windows.h>
#else
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <sys/mman.h>
#endif

#include "../source/arithmetic_coding.h"
#include "../source/pregen.h"
#include "../source/string_pair.h"

#ifdef _MSC_VER
	char *map_entire_file(const char *filename, long long *filesize)
	{
	HANDLE mapping;
	HANDLE fp;
	char *result;

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

	if (fd == -1)
		return NULL;

	fstat(fd, &buffer);

	result =(char*) mmap (0, buffer.st_size, PROT_READ, MAP_SHARED, fd, 0);

	if (result==MAP_FAILED)
		return NULL;

	return result;
	}
#endif

int main(int argc, char**argv)
{
char *doclist_filename, *doclist_field, *model_name;
enum { MODEL_BASE37, MODEL_PRINTABLES} model_type;

if (argc < 4)
	{
	fprintf(stderr, "Usage: %s <doclistfile> <field> <model>\n(model is one of base37 or printables)\n", argv[0]);
	exit(-1);
	}

doclist_filename = argv[1];
doclist_field = argv[2];
model_name = argv[3];

if (strcmp(model_name, "base37") == 0)
	model_type = MODEL_BASE37;
else if (strcmp(model_name, "printables") == 0)
	model_type = MODEL_PRINTABLES;
else
	{
	fprintf(stderr, "Bad model\n");
	exit(-1);
	}

ANT_arithmetic_model model(model_type == MODEL_BASE37 ? ANT_encode_char_base37::num_symbols : ANT_encode_char_printable_ascii::num_symbols, NULL, 0);

//Read document names from the .doclist file
char *doclist = map_entire_file(doclist_filename, NULL);
char *current_document;

if (!doclist)
	{
	fprintf(stderr, "Couldn't read doclist\n");
	exit(-1);
	}

current_document = doclist;

while (*current_document)
	{
	char * docnameend = strchr(current_document, '\n');
	char * pos = current_document;
	enum { IDLE, INSIDE_TAG, INSIDE_OPEN_TAG, INSIDE_CLOSE_TAG} state = IDLE;

	if (!docnameend)
		break;

	int tag_depth = 0;
	ANT_string_pair tag_name, tag_body, close_tag_name;

	/* Parse document name and find matching top-level XML fields to use */
	while (pos < docnameend)
		{
		switch (state)
			{
			case IDLE:
				if (*pos == '<')
					{
					tag_body.string_length = pos - tag_body.start;
					state = INSIDE_TAG;
					}
				break;
			case INSIDE_TAG:
				if (*pos == '/')
					{
					state = INSIDE_CLOSE_TAG;
					close_tag_name.start = pos + 1;
					break;
					}

				tag_name.start = pos;
				state = INSIDE_OPEN_TAG;
				//Fall-through

			case INSIDE_OPEN_TAG:
				if (*pos == '>')
					{
					tag_name.string_length = pos - tag_name.start;
					tag_body.start = pos + 1;
					tag_depth++;
					state = IDLE;
					}
				break;

			case INSIDE_CLOSE_TAG:
				if (*pos == '>')
					{
					tag_depth--;

					if (tag_depth == 0)
						{
						if (tag_name.true_strcmp(doclist_field) == 0)
							{
							for (int i = 0; i < tag_body.length(); i++)
								{
								char c = tag_body[i];
								unsigned char symbol;

								switch (model_type)
									{
									case MODEL_BASE37:
										symbol = ANT_encode_char_base37::encode(c);
										break;
									case MODEL_PRINTABLES:
										/* Lowercase text before encoding */
										if (c >= 'A' && c <= 'Z')
											c += ('a' - 'A');

										symbol = ANT_encode_char_printable_ascii::encode(c);
										break;
									default:
										assert(0);
									}

								if (symbol != CHAR_ENCODE_FAIL)
									model.update(symbol);
								}
							}
						state = IDLE;
						}
					}
				break;
			}
		++pos;
		}

	current_document = docnameend + 1;
	}

int col = 0;

for (int i = 0; i < model.nsym; i++)
	{
	printf(i == model.nsym - 1 ? "%d" : "%d, ", model.freq[i]);

	col++;
	if (col % 16 == 0)
		printf("\n");
	}
printf("\n");

return 0;
}

