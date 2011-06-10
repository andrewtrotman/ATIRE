/*
	MAKE_UNICODE_TABLES.C
	----------------------------
	This program reads the UnicodeData.txt file (see:http://unicode.org/Public/UNIDATA/UnicodeData.txt) and generates
	a language independant uppercase and lowercase conversion table. For details on the format of UnicodeData.txt
	see (http://www.unicode.org/Public/5.1.0/ucd/UCD.html#UnicodeData.txt).
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <cassert>
#include "../source/unicode.h"
#include "../source/unicode_pair.h"
#include "../source/disk.h"

enum { LOWER, UPPER, DECOMPOSITION, CHARTYPE };

#define NUM_UNICODE_CHARS 0x110000

unsigned char chartypes[NUM_UNICODE_CHARS];
unsigned long *decompositions[NUM_UNICODE_CHARS], lowercase[NUM_UNICODE_CHARS], uppercase[NUM_UNICODE_CHARS];

/*
 * Recursively decompose the given character into the buffer pointed to by buf. When done, buf points after the end of the
 * decomposition.
 *
 * Return non-zero if the character decomposes to something other than itself.
 */
int decompose(unsigned long c, unsigned long ** buf)
{
assert(c < NUM_UNICODE_CHARS);

if (decompositions[c])
	{
	unsigned long * decomp = decompositions[c];
	int decomposed = 0;

	while (*decomp)
		{
		decomposed = decomposed || decompose(*decomp, buf);
		++decomp;
		}
	return decomposed;
	}
else
	{
	ANT_UNICODE_chartype chartype = (ANT_UNICODE_chartype) chartypes[c];

	if (chartype != CT_MARK && chartype != CT_PUNCTUATION && chartype != CT_SEPARATOR)
		{
		//This character decomposes to itself
		**buf = c;
		*buf++;
		return 0;
		}
	else
		{
		/* This character decomposes to itself but it's not a character we care about.
		 * Leave it out.
		 */
		return 1;
		}
	}
}

/* Iterate over the decompositions and recursively apply them to get maximally decomposed
 * characters, throwing out characters that we don't care about (punctuation, combining marks, etc.).
 */
void reduce_decompositions() {

	int made_any_change;

	do
		{
		made_any_change = 0;
		for (int i=0; i<NUM_UNICODE_CHARS; i++)
			if (decompositions[i])
				{
				unsigned long buf[1000];
				unsigned long *buf_pos;
				unsigned long *src_pos;

				buf_pos = buf;

				//Attempt to decompose every code-point in this decomposition further
				int made_change = 0;
				for (src_pos = decompositions[i]; *src_pos; src_pos++)
					if (decompose(*src_pos, &buf_pos))
						made_change = 1;

				if (made_change)
					{
					made_any_change = 1;
					delete [] decompositions[i];
					decompositions[i] = new unsigned long[buf_pos - buf +1];

					memcpy(decompositions[i], buf, (buf_pos - buf) * sizeof(decompositions[0]));
					decompositions[buf_pos-buf] = 0;
					}
				for (unsigned long * p = decompositions[i]; p && *p; p++)
					if (lowercase[*p])
						{
						made_change = 1;
						*p=lowercase[*p];
						}
				}
			fprintf(stderr, "Reduced decomposition table recursively.\n");
		} while (made_any_change);
}

void print_usage(char * argv[])
{
exit(printf("Usage:%s <-lower | -upper | -decomposition | -chartype> <UnicodeData.txt>\n", argv[0]));
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
unsigned long decomposition[640], *decomposed_pos;
char *file, *pos, *end;
char **lines, **current;
long long number_of_lines;
long field, mode, times;
unsigned long upper_character, lower_character, character, destination_character;
int state;
enum ANT_UNICODE_chartype chartype;
char utf8_buf[100];
char *utf8_buf_ptr;

if (argc != 3)
	print_usage(argv);

if (strcmp(argv[1], "-lower") == 0)
	mode = LOWER;
else if (strcmp(argv[1], "-upper") == 0)
	mode = UPPER;
else if (strcmp(argv[1], "-decomposition") == 0)
	mode = DECOMPOSITION;
else if (strcmp(argv[1], "-chartype") == 0)
	mode = CHARTYPE;
else
	{
	mode = LOWER; //Silence uninitialized warning from dumb compilers
	print_usage(argv);
	}

if ((file = ANT_disk::read_entire_file(argv[2])) == NULL)
	exit(printf("Cannot open input file:%s\n", argv[2]));

lines = ANT_disk::buffer_to_list(file, &number_of_lines);

memset(chartypes, CT_OTHER, sizeof(chartypes));
memset(lowercase, 0, sizeof(lowercase));
memset(uppercase, 0, sizeof(uppercase));

memset(decompositions, 0, sizeof(decompositions));

//Load in info from tables...
for (current = lines; *current != NULL; current++)
	{
	if (**current == '\0' || isspace(**current))
		continue;
	pos = *current;

	// Character code for this line
	sscanf(pos, "%x", &character);
	pos = strchr(pos, ';') + 1;

	// Skip character name field
	pos = strchr(pos, ';') + 1;

	// Character classification
	end = strchr(pos, ';');
	chartype = CT_OTHER;
	if (end > pos)
		switch (*pos)
		{
			case 'L':
				chartype = CT_LETTER;
				break;
			case 'M':
				chartype = CT_MARK;
				break;
			case 'N':
				chartype = CT_NUMBER;
				break;
			case 'P':
			case 'S':
				chartype = CT_PUNCTUATION;
				break;
			case 'Z':
				chartype = CT_SEPARATOR;
				break;
			default:
				chartype = CT_OTHER;
		}
	chartypes[character] = (unsigned char) chartype;
	pos = end + 1;

	// Skip some fields...
	for (field = 0; field < 2; field++)
		pos = strchr(pos, ';') + 1;

	// Character decomposition
	end = strchr(pos, ';');

	decomposed_pos = decomposition;

	state = 0;
	while (*pos != ';')
		{
		switch (state)
			{
			case 0:
				if (*pos == '<')
					state = 1;
				else if (isalnum(*pos))
					{
					int c;
					assert(sscanf(pos, "%x", &c)==1);

					*decomposed_pos = c;
					decomposed_pos++;

					state = 2;
					}
				break;
			case 1:
				if (*pos == '>')
					state = 0;
				break;
			case 2:
				if (!isalnum(*pos))
					state = 0;
				break;
			}
		++pos;
		}
	pos = end + 1;

	for (field = 0; field < 7; field++)
		pos = strchr(pos, ';') + 1;

	upper_character = lower_character = 0;
	sscanf(pos, "%x", &lower_character);
	pos = strchr(pos, ';') + 1;
	sscanf(pos, "%x", &upper_character);

	lowercase[character] = lower_character;
	uppercase[character] = upper_character;

	if (decomposed_pos > decomposition)
		{
		//This character decomposes to something other than itself. Store that decomposition into the list.
		decompositions[character] = new unsigned long[decomposed_pos - decomposition + 1];
		memcpy(decompositions[character], decomposition, sizeof(decomposition[0] * (decomposed_pos - decomposition)));

		//Null-terminate
		decompositions[decomposed_pos - decomposition] = 0;
		}
	else if (lower_character)
		{
		/* Does this character have a lowercase variant? We won't be decomposing it but we at least want an
		 * entry in the table for lowercasing
		 */
		decompositions[character] = new unsigned long[2];
		decompositions[character][0] = lower_character;
		decompositions[character][1] = 0;
		}
	}

reduce_decompositions();

switch (mode)
	{
	case UPPER:
		printf("ANT_UNICODE_pair ANT_UNICODE_upper[] = {\n");
		break;
	case LOWER:
		printf("ANT_UNICODE_pair ANT_UNICODE_lower[] = {\n");
		break;
	case DECOMPOSITION:
		printf("ANT_UNICODE_decomposition ANT_UNICODE_decomposition[] = {\n");
		break;
	case CHARTYPE:
		printf("unsigned char ANT_UNICODE_char_chartype[] = {\n");
		break;
	}


// Now print tables
times = 0;
for (current = lines; *current != NULL; current++)
	{
	if (**current == '\0' || isspace(**current))
		continue;

	// Character code for this line
	sscanf(*current, "%x", &character);

	if (mode == LOWER || mode == UPPER)
		{
		if (mode == LOWER)
			destination_character = lowercase[character];
		else
			destination_character = uppercase[character];

		if (destination_character != 0)
			{
			if (times != 0)
				{
				printf(", ");
				if (times % 16 == 0)
					printf("\n");
				}
			printf("{%ld, %ld}", character, destination_character);
			times++;
			}
		}
	else if (mode == DECOMPOSITION && decompositions[character])
		{
		if (times != 0)
			{
			printf(", ");
			if (times % 16 == 0)
				printf("\n");
			}

		utf8_buf_ptr = utf8_buf;
		unsigned long * wide_ptr = decompositions[character];

		while (*wide_ptr)
			{
			//Convert the decomposition to UTF8 so we can write it into our UTF8 sourcecode

			// Apply C-escaping to characters which need it
			if (*wide_ptr == '"')
				{
				*utf8_buf_ptr = '\\';
				utf8_buf_ptr++;
				*utf8_buf_ptr = '\"';
				utf8_buf_ptr++;
				}
			else if (*wide_ptr == '\\')
				{
				*utf8_buf_ptr = '\\';
				utf8_buf_ptr++;
				*utf8_buf_ptr = '\\';
				utf8_buf_ptr++;
				}
			else
				utf8_buf_ptr += wide_to_utf8(utf8_buf_ptr, 100, *wide_ptr);

			wide_ptr++;
			}
		*utf8_buf_ptr = 0;
		printf("{%ld, \"%s\"}", character, utf8_buf);
		times++;
		}
	else if (mode == CHARTYPE)
		{
		chartype = (ANT_UNICODE_chartype) chartypes[character];

		switch (chartype)
			{
			case CT_PUNCTUATION:
			case CT_SEPARATOR:
			case CT_NUMBER:
			case CT_LETTER:
				if (times != 0)
					{
					printf(", ");
					if (times % 16 == 0)
						printf("\n");
					}

				printf("{%ld, (unsigned char)(%s%s)}", character, ANT_UNICODE_chartype_string[(int) chartype],
						ischinese(character) ? " | CT_CHINESE" : "");

				times++;
				break;
			default:
				if (ischinese(character))
					{
					if (times != 0)
						{
						printf(", ");
						if (times % 16 == 0)
							printf("\n");
						}

					printf("{%ld, (unsigned char) (CT_OTHER | CT_CHINESE)}", character);

					times++;
					}
				break;
			}
		}
	}
printf("\n};\n");


return 0;
}
