/*
	LINK_LENGTH_CORRELATE.C
	-----------------------
	Written (w) 2008 by Andrew Trotman, University of Otago
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

#define MAX_ALLOWABLE_LINKS 10000000
class ANT_link_answer
{
public:
	long orphan, target, anchor_terms, anchor_strlen, gamma;
	long relevant;
public:
	static int compare(const void *a, const void *b);
} ;

ANT_link_answer all_known_links[MAX_ALLOWABLE_LINKS];
long all_known_links_length;
char buffer[1024 * 1024];

/*
	ANT_LINK_ANSWER::COMPARE()
	--------------------------
*/
int ANT_link_answer::compare(const void *a, const void *b)
{
ANT_link_answer *one, *two;
int cmp;

one = (ANT_link_answer *)a;
two = (ANT_link_answer *)b;

if ((cmp = one->orphan - two->orphan) == 0)
	cmp = one->target - two->target;

return cmp;
}

/*
	USAGE()
	-------
*/
void usage(char *exename)
{
fprintf(stderr, "Usage:%s <lnkfile><assessmentfile>\n", exename);
exit(1);
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
FILE *fp;
ANT_link_answer *current, key;
long current_topic, current_target;


if (argc < 3)
	usage(argv[0]);
if ((fp = fopen(argv[1], "rb")) == NULL)
	usage(argv[0]);
/*
	read the run's answers
*/
all_known_links_length = 0;
current = all_known_links;
while (fgets(buffer, sizeof(buffer), fp) != NULL)
	{
	sscanf(buffer, "%d:%d:%d:%d:%d", &current->orphan, &current->target, &current->anchor_terms, &current->anchor_strlen, &current->gamma);
	current->relevant = FALSE;
	current++;
	all_known_links_length++;
	}
fclose(fp);
qsort(all_known_links, all_known_links_length, sizeof(*all_known_links), ANT_link_answer::compare);

/*
	read the assessments
*/
if ((fp = fopen(argv[2], "rb")) == NULL)
	usage(argv[0]);

current_topic = current_target = 0;
while (fgets(buffer, sizeof(buffer), fp) != NULL)
	{
	_strlwr(buffer);
	if (strstr(buffer, "<ltw_topic") != 0)
		current_topic = atol(strstr(buffer, "id=\"") + 4);
	else if (strstr(buffer, "<outlink") != 0)
		{
		current_target = atol(strchr(buffer, '>') + 1);

		key.orphan = current_topic;
		key.target = current_target;
		current = (ANT_link_answer *)bsearch(&key, all_known_links, all_known_links_length, sizeof(*all_known_links), ANT_link_answer::compare);
		if (current != NULL)
			current->relevant = TRUE;
		}
	}
fclose(fp);
/*
	output the answer
*/
for (current = all_known_links; current < all_known_links + all_known_links_length; current++)
	printf("%d\t%d\t%d\t%d\n", current->anchor_terms, current->anchor_strlen, current->gamma, current->relevant);

return 0;
}
