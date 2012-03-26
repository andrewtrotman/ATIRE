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
if (argc != 4)
	exit(printf("Usage: %s <doclist file> <pregen source> <pregen name>\n", argv[0]));

ANT_file *pregen = new ANT_file;
char file_header[] = "ANT Search Engine Pregen File\n\0";
char *pregen_filename = new char[strlen("index.aspt.") + strlen(argv[3]) + 1];
strcpy(pregen_filename, "index.aspt.");
strcat(pregen_filename, argv[3]);

long long lines, low, mid, high, docs = 0;
uint32_t four_byte;
ANT_pregen_t rsv;
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
		rsv = (ANT_pregen_t)ANT_atol(++p);
		}
	else
		{
		rsv = 1;
		fprintf(stderr, "Warning: didn't find score for %s, setting to 1\n", line);
		}
	pregen->write((unsigned char *)&rsv, sizeof(rsv));
	}

four_byte = PREGEN_FILE_VERSION;
pregen->write((unsigned char *)&four_byte, sizeof(four_byte));
four_byte = (uint32_t)docs;
pregen->write((unsigned char *)&four_byte, sizeof(four_byte));
four_byte = sizeof(ANT_pregen_t);
pregen->write((unsigned char *)&four_byte, sizeof(four_byte));
four_byte = (uint32_t)INTEGER;
pregen->write((unsigned char *)&four_byte, sizeof(four_byte));
four_byte = (uint32_t)strlen(argv[3]);
pregen->write((unsigned char *)&four_byte, sizeof(four_byte));

pregen->close();

return EXIT_SUCCESS;
}
