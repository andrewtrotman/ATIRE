/*
	QREL_ORIG_ALT_COMPARE.C
	-----------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../source/disk.h"

#define FALSE 0
#define TRUE (!FALSE)
/*
	class QREL
	----------
*/
class qrel
{
public:
	long topic;
	long document;
	long hit;
} ;

/*
	REL_CMP()
	---------
*/
int rel_cmp(const void *a, const void *b)
{
qrel *one, *two;

one = (qrel *)a;
two = (qrel *)b;

if (one->topic < two->topic)
	return -1;
else if (one->topic > two->topic)
	return 1;
else if (one->document < two->document)
	return -1;
else if (one->document > two->document)
	return 1;
else
	return 0;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
char *file1, *file2;
char **lines1, **lines2, **current;
qrel *rels1, *rels2, *current_rel;
long long length1, length2;
long intersect, non_intersect, current_topic;

if (argc != 3)
	exit(printf("usage:%s <origqelfile2> <altqrelfile2>\n", argv[0]));

file1 = ANT_disk::read_entire_file(argv[1]);
file2 = ANT_disk::read_entire_file(argv[2]);

lines1 = ANT_disk::buffer_to_list(file1, &length1);
lines2 = ANT_disk::buffer_to_list(file2, &length2);

if (file1 == NULL || file2 == NULL || lines1 == NULL || lines2 == NULL)
	exit(printf("Cannot read one of the input files\n"));

current_rel = rels1 = new qrel[length1 + 1];
for (current = lines1; *current != NULL; current++)
	{
	sscanf(*current, "%ld %ld", &(current_rel->topic), &(current_rel->document));
	current_rel->hit = FALSE;
	current_rel++;
	}
qsort(rels1, length1, sizeof(*rels1), rel_cmp);

current_rel = rels2 = new qrel[length2 + 1];
for (current = lines2; *current != NULL; current++)
	{
	sscanf(*current, "%ld %ld", &(current_rel->topic), &(current_rel->document));
	current_rel->hit = FALSE;
	current_rel++;
	}
qsort(rels2, length2, sizeof(*rels2), rel_cmp);

/*
	Find which of rel2 are in rel1 and which are not
*/
for (current_rel = rels2; current_rel < rels2 + length2; current_rel++)
	if (bsearch(current_rel, rels1, length1, sizeof(*rels1), rel_cmp) != NULL)
		current_rel->hit = TRUE;

current_topic = -1;
intersect = non_intersect = 0;
for (current_rel = rels2; current_rel < rels2 + length2; current_rel++)
	{
	if (current_topic != current_rel->topic)
		{
		if (current_topic >= 0)
			printf("%ld %ld %ld\n", current_topic, intersect, non_intersect);
		current_topic = current_rel->topic;
		intersect = 0;
		non_intersect = 0;
		}

	if (current_rel->hit)
		intersect++;
	else
		non_intersect++;
	}

if (current_topic >= 0)
	printf("%ld %ld %ld\n", current_topic, intersect, non_intersect);

return 0;
}
