/*
	MKQREL.C
	--------
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

char buffer[1024 * 1024];

typedef struct
{
long id;
char *name;
} doc_id_pair;

doc_id_pair docid_list[1024*1024];

/*
	USAGE()
	-------
*/
void usage(char *exename)
{
exit(printf("Usage:%s <doc_id_file> <qrel_file>\n", exename));
}

/*
	KEY_COMPARE()
	-------------
*/
int key_compare(const void *a, const void *b)
{
doc_id_pair *one, *two;

one = (doc_id_pair *)a;
two = (doc_id_pair *)b;

return strcmp(one->name, two->name);
}

/*
	STRIP_END_PUNC()
	----------------
*/
char *strip_end_punc(char *buffer)
{
char *ch;

for (ch = buffer + strlen(buffer) - 1; ch > buffer; ch--)
	if (isspace(*ch))
		*ch = '\0';
	else
		break;

return buffer;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
FILE *fp, *qrel;
char *ch;
long current, ids, topic;
doc_id_pair key, *got;

if (argc != 3)
	usage(argv[0]);

if ((fp = fopen(argv[1], "rb")) == NULL)
	exit(printf("Cannot open doc_id_file:%s\n", argv[1]));

ids = 0;
while (fgets(buffer, sizeof(buffer), fp) != NULL)
	{
	strip_end_punc(buffer);
	docid_list[ids].id = ids;
	docid_list[ids].name = strdup(buffer);
	ids++;
	}
fclose(fp);

qsort(docid_list, ids, sizeof(key), key_compare);

if ((qrel = fopen(argv[2], "rb")) == NULL)
	exit(printf("Cannot open qrel_file:%s\n", argv[1]));

while (fgets(buffer, sizeof(buffer), fp) != NULL)
	{
	topic = atol(buffer);
	if (topic == 0)
		exit(printf("Badly formed qrel - topic id must not be 0\n"));
	if ((ch = strchr(buffer, ' ')) == NULL)
		exit(printf("Badly formed qrel - must be in the form <topic> <docid>\n"));

	key.name = strip_end_punc(ch + 1);
	got = (doc_id_pair *)bsearch(&key, docid_list, ids, sizeof(key), key_compare);
	if (got == NULL)
		exit(printf("Docid '%s' not in <doc_id_file>\n", key.name));

	printf("%ld %ld\n", topic, got->id);
	}

fclose(qrel);

return 0;
}
