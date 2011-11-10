/*
	GETSYNS.C
	---------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "/ant/source/disk.h"

/*
	GET_TITLE()
	-----------
*/
char *get_title(char *line)
{
char *term, *term_end;
long long times, lex_id, current;

term = line;
term = strchr(term, ' ') + 1;
term = strchr(term, ' ') + 1;
term = strchr(term, ' ') + 1;

times = atol(term);

if (times > 1)
	printf("(");

for (current = 0; current < times; current++)
	{
	term = strchr(term, ' ') + 1;
	term_end = strchr(term, ' ');

	if (current != 0)
		printf(" ");
	printf("%*.*s", term_end - term, term_end - term, term);

	term = term_end + 1;
	lex_id = atol(term);
	}
if (times > 1)
	printf(")");

return NULL;
}

/*
	LIST_SYNSETS()
	--------------
*/
void list_synsets(char *file, char *at, long long times)
{
char type[4], pos;
long long offset;
int source_target;

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
		printf("   [%d]", offset);
		get_title(file + offset);
		puts("");
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
	int w_cnt, current_word;
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

	get_title(*current);

	for (current_word = 0; current_word < w_cnt; current_word++)
		{
		at = strchr(at, ' ') + 1;
		at = strchr(at, ' ') + 1;
		}
	 
	p_cnt = atol(at);
	if ((at = strchr(at, ' ')) == NULL)
		continue;
	at++;

	printf(": %d synsets\n", p_cnt);
	list_synsets(file, at, p_cnt);
	}

delete [] lines;
delete [] file;

return 0;
}