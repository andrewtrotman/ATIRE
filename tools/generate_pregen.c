/*
	GENERATE_PREGEN.C
	-----------------
	Generates a pregen of a given name from a file containing <docid> <score> lines for documents in a given doclist file

	Assumes that the file containing the scores is sorted on docid
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../source/pregen_field_type.h"
#include "../source/pregen_t.h"
#include "../source/file.h"
#include "../source/disk.h"
#include "../source/ctypes.h"
#include "../source/str.h"

/*
	MAIN()
	------
*/
int main(int argc, char **argv)
{
if (argc < 4)
	exit(printf("Usage: %s <doclist file> <pregen source> <pregen name> [bytes default=8]\n", argv[0]));

ANT_file *pregen = new ANT_file;
char file_header[] = "ANT Search Engine Pregen File\n\0";
char *pregen_filename = new char[strlen("index.aspt.") + strlen(argv[3]) + 1];
strcpy(pregen_filename, "index.aspt.");
strcat(pregen_filename, argv[3]);

long long bytes;
if (argc > 4)
	bytes = strtod(argv[4], (char **)NULL);
else
	bytes = 8;

long long lines, low, mid, high, docs = 0;
long long parsed, rsv;
uint64_t eight_byte;
uint32_t four_byte;
uint16_t two_byte;
uint8_t one_byte;
char line[4096];
FILE *doclist = fopen(argv[1], "rb");
size_t docid_len;
char *p;

char *score_source = ANT_disk::read_entire_file(argv[2]);
char **scores = ANT_disk::buffer_to_list(score_source, &lines);

pregen->open(pregen_filename, "w");
pregen->write((unsigned char *)file_header, sizeof(file_header));
pregen->write((unsigned char *)argv[3], strlen(argv[3]) * sizeof(*argv[3]));

while (fgets(line, sizeof(line), doclist) != NULL)
	{
	low = 0;
	high = lines - 1;
	strip_space_inplace(line);
	docid_len = strlen(line);
	docs++;

	while (low < high)
		{
		mid = low + ((high - low) / 2);
		if (strncmp(scores[mid], line, docid_len) < 0)
			low = mid + 1;
		else
			high = mid;
		}

	if (strncmp(scores[low], line, docid_len) == 0)
		{
		p = scores[low];
		while (!ANT_isspace(*p)) p++; // find the space
		p++; // skip over the space

		parsed = ANT_atoi64(p);
		//parsed = (ANT_atoi64(p) / 2) + 1;
		switch (bytes)
			{
			case 8:
				rsv = (uint64_t)parsed;
				break;
			case 4:
				rsv = (uint32_t)parsed;
				break;
			case 2:
				rsv = (uint16_t)parsed;
				break;
			case 1:
				rsv = (uint8_t)parsed;
				break;
			}
		//rsv = (uint16_t)((ANT_atol(p) / 2) + 1); // convert to signed 2 byte entity while preserving non-0
		}
	else
		{
		rsv = 1;
		fprintf(stderr, "Warning: didn't find score for %s, setting to 1\n", line);
		}
		
	switch (bytes)
		{
		case 8:
			pregen->write((unsigned char *)&rsv, sizeof(eight_byte));
			break;
		case 4:
			pregen->write((unsigned char *)&rsv, sizeof(four_byte));
			break;
		case 2:
			pregen->write((unsigned char *)&rsv, sizeof(two_byte));
			break;
		case 1:
			pregen->write((unsigned char *)&rsv, sizeof(one_byte));
			break;
		}
	}

four_byte = PREGEN_FILE_VERSION;
pregen->write((unsigned char *)&four_byte, sizeof(four_byte));
four_byte = (uint32_t)docs;
pregen->write((unsigned char *)&four_byte, sizeof(four_byte));
four_byte = bytes;
pregen->write((unsigned char *)&four_byte, sizeof(four_byte));
four_byte = (uint32_t)INTEGER;
pregen->write((unsigned char *)&four_byte, sizeof(four_byte));
four_byte = (uint32_t)strlen(argv[3]);
pregen->write((unsigned char *)&four_byte, sizeof(four_byte));

pregen->close();

return EXIT_SUCCESS;
}
