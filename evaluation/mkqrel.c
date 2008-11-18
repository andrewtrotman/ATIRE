/*
	MKQREL.C
	--------
*/
#include <stdio.h>
#include <string.h>
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
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
FILE *fp;
char *ch;
long current, ids;

if (argc != 3)
	usage(argv[0]);

if ((fp = fopen(argv[1], "rb")) == NULL)
	exit(printf("Cannot open doc_id_file:%s\n", argv[1]));

ids = 0;
while (fgets(buffer, sizeof(buffer), fp) != NULL)
	{
	for (ch = buffer + strlen(buffer) - 1; ch > buffer; ch--)
		if (isspace(*ch))
			*ch = '\0';
		else
			break;

	docid_list[ids].id = ids;
	docid_list[ids].name = strdup(buffer);
	ids++;
	}

for (current = 0; current < ids; current++)
	printf("<%d>: %s\n", docid_list[current].id, docid_list[current].name);

return 0;
}
