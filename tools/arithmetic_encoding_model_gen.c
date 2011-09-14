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
#include "../source/string_pair.h"

#define NUM_SYMBOLS (26 + 10 + 1)

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
ANT_arithmetic_model model(NUM_SYMBOLS, NULL, 0);
char *doclist;

assert(argc > 1);

//Read document names from the .doclist file
doclist = map_entire_file(argv[1], NULL);

if (!doclist)
	{
	fprintf(stderr, "Couldn't read doclist\n");
	exit(-1);
	}

char *current_document = doclist;
while (*current_document)
	{
	char * docnameend = strchr(current_document, '\n');

	if (!docnameend)
		break;

	char * pos = current_document;
	enum { IDLE, INSIDE_TAG, INSIDE_OPEN_TAG, INSIDE_CLOSE_TAG} state = IDLE;

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
						if (tag_name.true_strcmp("title") == 0)
							{
							for (int i = 0; i < tag_body.length(); i++)
								{
								char c = tag_body[i];
								int symbol;

								if (c == ' ')
									symbol = 0;
								else if (c >= '0' && c <= '9')
									symbol = c - '0' + 1;
								else if (c >= 'a' && c <= 'z')
									symbol = c - 'a' + 1 + 10;
								else if (c >= 'A' && c <= 'Z')
									symbol = c - 'A' + 1 + 10;
								else
									continue;

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

for (int i = 0; i < model.nsym; i++)
	{
	printf("%d\n", model.freq[i]);
	}

return 0;
}

