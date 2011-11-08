/*
	GETSYNS.C
	---------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "/ant/source/disk.h"

/*
	LIST_SYNSETS()
	--------------
*/
void list_synsets(char *file, char *at, long long times)
{
char type[4], pos;
long long offset, source_target;

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

	if (*type == '~')		// Hyponym
		{
		char *term, *term_end;

		printf("   (%d)", offset);

		term = file + offset;
		term = strchr(term, ' ') + 1;
		term = strchr(term, ' ') + 1;
		term = strchr(term, ' ') + 1;
		term = strchr(term, ' ') + 1;
		term_end = strchr(term, ' ') + 1;

		printf("%*.*s\n", term_end - term, term_end - term, term);
		}
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
	long long w_cnt;
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

	sscanf(at, "%s", word);
	if ((at = strchr(at, ' ')) == NULL)
		continue;
	at++;

	sscanf(at, "%1x", &lex_id);
	if ((at = strchr(at, ' ')) == NULL)
		continue;
	at++;

	p_cnt = atol(at);
	if ((at = strchr(at, ' ')) == NULL)
		continue;
	at++;

	printf("%s : %d synsets\n", word, p_cnt);
	list_synsets(file, at, p_cnt);
	}

delete [] lines;
delete [] file;

return 0;
}