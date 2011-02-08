/*
	LINK_ANALYSE_INEX_WIKIPEDIA.C
	-----------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../source/disk.h"
#include "../source/file.h"

#define PAGERANK_D 0.85
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
	static int cmp_on_source(const void *a, const void *b);
} ;

/*
	class ANT_link_node
	-------------------
*/
class ANT_link_node
{
public:
	ANT_link *node;
	ANT_link *inlinks, *outlinks;
	long indegree;
	long outdegree;
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
	ANT_LINK::CMP_ON_SOURCE()
	-------------------------
*/
int ANT_link::cmp_on_source(const void *a, const void *b)
{
ANT_link *one, *two;

one = (ANT_link *)a;
two = (ANT_link *)b;

if (one->source < two->source)
	return -1;
else if (one->source > two->source)
	return 1;
else if (one->destination < two->destination)
	return -1;
else if (one->destination > two->destination)
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
	COUNT_NODES()
	-------------
*/
long count_nodes(ANT_link *link_graph, long long size)
{
ANT_link *current, *end;
long last_source, targets;

end = link_graph + size;
targets = 0;
last_source = link_graph->destination;
for (current = link_graph + 1; current < end; current++)
	if (current->destination != last_source)
		{
		targets++;
		last_source = current->destination;
		}
targets++;

return targets;
}

/*
	MAKE_NODES()
	------------
*/
ANT_link_node *make_nodes(ANT_link *link_graph, long long size, long long *number_of_nodes)
{
ANT_link *current, *end;
long last_source, times;
ANT_link_node *current_node, *nodes;

*number_of_nodes = count_nodes(link_graph, size);
current_node = nodes = new ANT_link_node[(size_t)(*number_of_nodes + 1)];

end = link_graph + size;
last_source = link_graph->destination;
current_node->node = link_graph;
current_node->inlinks = link_graph;
current_node->outdegree = 0;
times = 1;
for (current = link_graph + 1; current < end; current++)
	{
	if (current->destination != last_source)
		{
		current_node->indegree = times;
		current_node++;
		current_node->node = current;
		current_node->inlinks = current;
		current_node->outdegree = 0;
		last_source = current->destination;
		times = 1;
		}
	else
		times++;
	}
current_node->indegree = times;

current_node++;
current_node->node = NULL;

return nodes;
}

/*
	COMPUTE_OUTDEGREE()
	-------------------
*/
ANT_link_node *compute_outdegree(ANT_link_node *nodes, ANT_link *outgraph, long number_of_nodes)
{
ANT_link_node *current;
ANT_link *outlinks, *end;
long times;

outlinks = outgraph;
end = outgraph + number_of_nodes;
for (current = nodes; current->node != NULL; current++)
	{
	while (outlinks < end && outlinks->source < current->node->source)
		outlinks++;

	times = 0;
	while (outlinks < end && outlinks->source == current->node->source)
		{
		times++;
		outlinks++;
		}

	current->outdegree = times;
	}

return nodes;
}

/*
	WRITE_GRAPH()
	-------------
*/
void write_graph(ANT_link_node *nodes)
{
ANT_link_node *current;

for (current = nodes; current->node != NULL; current++)
	printf("%ld has %ld incoming and %ld outgoing links\n", current->node->destination, current->indegree, current->outdegree);
}

/*
	COMPUTE_ONE_PAGERANK_ITERATION()
	--------------------------------
*/
void compute_one_pagerank_iteration(double *source, double *destination, ANT_link_node *nodes)
{
#ifdef NEVER
ANT_link *incoming;
ANT_link_node *current;
long which;
double sum;

for (which = 0, current = nodes; current->node != NULL; current++)
	{
	sum = 0.0;
	for (incoming = current->links; incoming->destination = current->node->destination; current++)
		sum += current

	which++;
	}
#endif
}

/*
	COMPUTE_PAGERANK()
	------------------
*/
double *compute_pagerank(long long total_nodes, long max_iterations, ANT_link_node *nodes)
{
double *current_iteration, *next_iteration, *node, *temp;
long iteration;

current_iteration = new double[total_nodes];
next_iteration = new double[total_nodes];

for (node = current_iteration; node < current_iteration + total_nodes; node++)
	*node = 1.0 / total_nodes;


for (iteration = 0; iteration < max_iterations; iteration++)
	{
	compute_one_pagerank_iteration(current_iteration, next_iteration, nodes);
	temp = next_iteration;
	next_iteration = current_iteration;
	current_iteration = temp;
	}

return current_iteration;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
long long total_links, number_of_nodes;
ANT_link *link_graph,*outdegree;
ANT_link_node *nodes;

if (argc < 2)
	exit(printf("Usage:%s <infile>...<infile>\n", argv[0]));

total_links = count_links(argc, argv);
link_graph = new ANT_link[(size_t)(total_links + 1)];

puts("Load link graph");
read_link_graph(argc, argv, link_graph);
link_graph[total_links].source = link_graph[total_links].destination = -1;

puts("Sort link graph on indegree");
qsort(link_graph, (size_t)total_links, sizeof(*link_graph), ANT_link::cmp_on_target);

puts("Compute indegree");
nodes = make_nodes(link_graph, total_links, &number_of_nodes);

outdegree = new ANT_link[(size_t)(total_links + 1)];
memcpy(outdegree, link_graph, sizeof(*link_graph) * (total_links + 1));

puts("Sort link graph on outdegree");
qsort(outdegree, (size_t)total_links, sizeof(*outdegree), ANT_link::cmp_on_source);

puts("Compute outdegree");
compute_outdegree(nodes, outdegree, total_links);

puts("Compute PageRank");
compute_pagerank(total_links, 2, nodes);

puts("Dump graph");
write_graph(nodes);
return 0;
}

