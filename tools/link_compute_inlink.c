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
void read_link_graph(int argc, char *argv[], ANT_link *link_graph)
{
long long current_file_size;
long long param;
unsigned char *into;

into = (unsigned char *)link_graph;
for (param = 1; param < argc; param++)
	{
	ANT_file file;
	file.open(argv[param], "rb");
	file.read(into, current_file_size = file.file_length());
	into += current_file_size;
	file.close();
	}
}

/*
	WRITE_INDEGREE()
	----------------
*/
long write_indegree(char *outfilename, ANT_link *link_graph, long long size)
{
ANT_file outfile;
ANT_link *current, *end;
long last_source, indegree;
long targets;

outfile.open(outfilename, "wb");
last_source = link_graph->destination;
indegree = 1;
end = link_graph + size;
targets = 0;
for (current = link_graph + 1; current < end; current++)
	{
	if (last_source != current->destination)
		{
		targets++;
		outfile.write((unsigned char *)&last_source, sizeof(last_source));
		outfile.write((unsigned char *)&indegree, sizeof(indegree));
//		printf("%ld <- %ld times\n", last_source, indegree);
		last_source = current->destination;
		indegree = 1;
		}
	else
		indegree++;
	}

targets++;
outfile.write((unsigned char *)&last_source, sizeof(last_source));
outfile.write((unsigned char *)&indegree, sizeof(indegree));
//printf("%ld <- %ld times\n", last_source, indegree);

outfile.close();

return targets;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
long long total_links, got;
ANT_link *link_graph;

if (argc < 3)
	exit(printf("Usage:%s <outfile> <infile>...<infile>\n", argv[0]));

total_links = count_links(argc - 1, argv + 1);
link_graph = new ANT_link[(size_t)total_links];
puts("Load link graph");
read_link_graph(argc - 1, argv + 1, link_graph);
puts("Sort link graph");
qsort(link_graph, (size_t)total_links, sizeof(*link_graph), ANT_link::cmp_on_target);
puts("write link graph");
got = write_indegree(argv[1], link_graph, total_links);

printf("%lld links to %lld targets\n", total_links, got);
return 0;
}

