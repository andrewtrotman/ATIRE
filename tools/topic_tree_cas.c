/*
	TOPIC_TREE_CAS.C
	----------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../source/str.h"
#include "../source/disk.h"

FILE *statsfile;

enum {TAG, TOPIC};

class ANT_node
{
public:
	long times;
	long node_number;
	long type;		// either a query or a node in the DOM tree
	char name[1024];
} nodes[1024];
long number_of_nodes = 0;

class ANT_link
{
public:
	long from;
	long to;
	long times;
} links[1024*1024];
long number_of_links = 0;

const char *SEPERATORS = "/ ";

char buffer[1024];

/*
	NODE_CMP()
	----------
*/
int node_cmp(const void *a, const void *b)
{
ANT_node *one, *two;
long cmp;

one = (ANT_node *)a;
two = (ANT_node *)b;

if ((cmp = one->type - two->type) == 0)
	cmp = strcmp(one->name, two->name);

return cmp;
}

/*
	LINK_CMP()
	----------
*/
int link_cmp(const void *a, const void *b)
{
ANT_link *one, *two;
long cmp;

one = (ANT_link *)a;
two = (ANT_link *)b;

if ((cmp = one->from - two->from) == 0)
	cmp = one->to - two->to;

return cmp;
}

/*
	FIND_NODE()
	-----------
*/
long find_node(char *name, long type)
{
long ans;
ANT_node *got = nodes;

nodes[number_of_nodes].times = 1;
nodes[number_of_nodes].type = type;
strcpy(nodes[number_of_nodes].name, name);
nodes[number_of_nodes].node_number = ans = number_of_nodes;

if (number_of_nodes == 0)
	number_of_nodes++;
else
	{
	got = (ANT_node *)bsearch(&nodes[number_of_nodes], nodes, number_of_nodes, sizeof(*nodes), node_cmp);
	if (got == NULL)
		{
		number_of_nodes++;
		qsort(nodes, number_of_nodes, sizeof(*nodes), node_cmp);
		}
	else
		{
		got->times++;
		ans = got->node_number;
		}
	}
return ans;
}

/*
	FIND_LINK()
	-----------
*/
void find_link(long from, long to)
{
ANT_link *got = links;

links[number_of_links].from = from;
links[number_of_links].to = to;
links[number_of_links].times = 1;

if (number_of_links == 0)
	number_of_links++;
else
	{
	got = (ANT_link *)bsearch(&links[number_of_links], links, number_of_links, sizeof(*links), link_cmp);
	if (got == NULL)
		{
		number_of_links++;
		qsort(links, number_of_links, sizeof(*links), link_cmp);
		}
	else
		got->times++;
	}
}

/*
	CLEAN_STRING()
	--------------
*/
void clean_string(char *string)
{
long in;
char *ch;

in = false;
for (ch = string; *ch != '\0'; ch++)
	if (in)
		{
		if (*ch == ']')
			in = false;
		*ch = ' ';
		}
	else
		if (*ch == '[')
			{
			*ch = ' ';
			in = true;
			}
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_disk disk;
char *file, **lines, *token;
long current;
long current_line, node, previous_node;
long long queries;

if (argc != 2)
	exit(printf("usage:%s <cas_query_file>\n", argv[0]));

file = disk.read_entire_file(argv[1]);
strlower(file);
lines = disk.buffer_to_list(file, &queries);

for (current_line = 0; current_line < queries; current_line++)
	{
	clean_string(lines[current_line]);
	previous_node = -1;
	for (token = strtok(lines[current_line], SEPERATORS); token != NULL; token = strtok(NULL, SEPERATORS))
		{
		node = find_node(token, TAG);
		if (previous_node != -1)
			find_link(previous_node, node);
		previous_node = node;
		}
	sprintf(buffer, "%d", current_line + 544);
	node = find_node(buffer, TOPIC);		// add the query itself
	find_link(previous_node, node);
	}

printf("*Vertices %d\n", number_of_nodes);
for (current = 0; current < number_of_nodes; current++)
	if (nodes[current].type == TAG)
		{
		long size = 5 + nodes[current].times / 10;
		printf("%d \"%s\" ellipse x_fact %d y_fact %d lr 0 ic Black lc White fos %d\n", nodes[current].node_number + 1, nodes[current].name, size, size, (long)(size * 2.6));
		}
	else
		printf("%d \"%s\" ellipse x_fact 4 y_fact 4 lr 0 s 0 ic White\n", nodes[current].node_number + 1, nodes[current].name);

printf("*Edges\n");
for (current = 0; current < number_of_links; current++)
	printf("%d %d %d c Black\n", links[current].from + 1, links[current].to + 1, links[current].times);

return 0;
}
