/*
	ARITHMETIC_ENCODING_MODEL_GEN.C
	-------------------------------
	Generate a character histogram from a specified field of a doclist file.
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
#include "../source/arithmetic_model_unigram.h"
#include "../source/arithmetic_model_bigram.h"
#include "../source/pregen.h"
#include "../source/string_pair.h"
#include "../source/encode_char_base37.h"
#include "../source/encode_char_base32.h"
#include "../source/encode_char_printable_ascii.h"

/*
	MAP_ENTIRE_FILE()
	-----------------
	Document collection is larger than my memory, so let's memory map it instead...
*/
#ifdef _MSC_VER
	char *map_entire_file(const char *filename, long long *filesize)
	{
	HANDLE mapping;
	HANDLE fp;
	char *result;

	(void)filesize;
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

/*
	MAIN()
	------
*/
int main(int argc, char**argv)
{
char *doclist_filename, *doclist_field, *model_name;
enum { MODEL_BASE32, MODEL_BASE37, MODEL_PRINTABLES} model_type;
int num_symbols;
int bigram = 0;
unsigned long long total_field_length = 0, documents_with_field = 0,
		unicode_bytes = 0;

if (argc < 4)
	{
	fprintf(stderr, "Usage: %s <doclistfile> <field> <model>\n(model is one of base32, base37, printables, base32bigram, base37bigram, printablesbigram)\n", argv[0]);
	exit(-1);
	}

doclist_filename = argv[1];
doclist_field = argv[2];
model_name = argv[3];

if (strcmp(model_name, "base32") == 0)
	model_type = MODEL_BASE32;
else if (strcmp(model_name, "base37") == 0)
	model_type = MODEL_BASE37;
else if (strcmp(model_name, "printables") == 0)
	model_type = MODEL_PRINTABLES;
else if (strcmp(model_name, "base32bigram") == 0)
	{
	model_type = MODEL_BASE32;
	bigram = 1;
	}
else if (strcmp(model_name, "base37bigram") == 0)
	{
	model_type = MODEL_BASE37;
	bigram = 1;
	}
else if (strcmp(model_name, "printablesbigram") == 0)
	{
	model_type = MODEL_PRINTABLES;
	bigram = 1;
	}
else
	{
	fprintf(stderr, "Bad model\n");
	exit(-1);
	}

switch (model_type)
	{
	case MODEL_BASE37:
		num_symbols = ANT_encode_char_base37::num_symbols;
		break;
	case MODEL_BASE32:
		num_symbols = ANT_encode_char_base32::num_symbols;
		break;
	case MODEL_PRINTABLES:
		num_symbols = ANT_encode_char_printable_ascii::num_symbols;
		break;
	default:
		return -1;
	}

ANT_arithmetic_model *model;

if (bigram)
	model = new ANT_arithmetic_model_bigram(num_symbols, NULL, 1);
else
	model = new ANT_arithmetic_model_unigram(num_symbols, NULL, 1);

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
							model->clear_context();

							total_field_length += tag_body.length();
							documents_with_field++;

							for (unsigned int i = 0; i < tag_body.length(); i++)
								{
								char c = tag_body[i];
								unsigned char symbol;

								if ((c & 0x80) != 0)
									unicode_bytes++;

								/* Lowercase text before encoding */
								if (c >= 'A' && c <= 'Z')
									c += ('a' - 'A');

								switch (model_type)
									{
									case MODEL_BASE32:
										symbol = ANT_encode_char_base32::encode(c);
									break;
									case MODEL_BASE37:
										symbol = ANT_encode_char_base37::encode(c);
										break;
									case MODEL_PRINTABLES:
										symbol = ANT_encode_char_printable_ascii::encode(c);
										break;
									default:
										return -1;
									}

								if (symbol != CHAR_ENCODE_FAIL)
									model->update(symbol);
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

model->text_render();

printf("Total chars examined: %llu, of which %llu are Unicode bytes\n", total_field_length, unicode_bytes);
printf("Average field length of %llu documents which contain the field '%s' is %llu\n", documents_with_field, doclist_field, total_field_length / documents_with_field);

return 0;
}

