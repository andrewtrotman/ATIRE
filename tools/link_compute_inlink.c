/*
	LINK_COMPUTE_INLINK.C
	---------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include "../source/disk.h"
#include "../source/file.h"

/*
	class ANT_LINK
	--------------
*/
class ANT_link
{
public:
	long source, destination;

public:
	static int cmp_on_target(const void *a, const void *b);
} ;

/*
	ANT_LINK::CMP_ON_TARGET()
	-------------------------
*/
int ANT_link::cmp_on_target(const void *a, const void *b)
{
ANT_link *one, *two;

one = (ANT_link *)a;
two = (ANT_link *)b;

if (one->destination < two->destination)
	return -1;
else if (one->destination > two->destination)
	return 1;
else if (one->source < two->source)
	return -1;
else if (one->source > two->source)
	return 1;
return 0;
}

/*
	COUNT_LINKS()
	-------------
*/
long long count_links(int argc, char *argv[])
{
long long total_links;
long long current_file_size;
long long param;

total_links = 0;
for (param = 1; param < argc; param++)
	{
	ANT_file file;
	file.open(argv[param], "rb");
	current_file_size = file.file_length();
	total_links += current_file_size / (sizeof(long) * 2);
	file.close();
	}
return total_links;
}

/*
	READ_LINK_GRAPH()
	-----------------
*/
long long read_link_graph(int argc, char *argv[], ANT_link *link_graph)
{
long long total_links;
long long current_file_size;
long long param;
unsigned char *into;

total_links = 0;
into = (unsigned char *)link_graph;
for (param = 1; param < argc; param++)
	{
	ANT_file file;
	file.open(argv[param], "rb");
	file.read(into, current_file_size = file.file_length());
	into += current_file_size;
	file.close();
	}
return total_links;
}

/*
	WRITE_INDEGREE()
	----------------
*/
long write_indegree(ANT_link *link_graph, long long size)
{
ANT_link *current, *end;
long last_source;
long indegree;

last_source = link_graph->destination;
indegree = 1;
end = link_graph + size;
for (current = link_graph + 1; current < end; current++)
	{
	if (last_source != current->destination)
		{
		printf("%ld <- %ld times\n", last_source, indegree);
		last_source = current->destination;
		indegree = 1;
		}
	else
		indegree++;
	if (last_source == 3)
		printf("%ld <- %ld\n", last_source, current->source);
		
	}
printf("%ld <- %ld times\n", last_source, indegree);

return size;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
long long total_links, got;
ANT_link *link_graph;

total_links = count_links(argc, argv);
link_graph = new ANT_link[(size_t)total_links];
puts("Load link graph");
got = read_link_graph(argc, argv, link_graph);
puts("Sort link graph");
qsort(link_graph, total_links, sizeof(*link_graph), ANT_link::cmp_on_target);

write_indegree(link_graph, total_links);

printf("Links found:%lld links read%lld\n", total_links, got);
return 0;
}

