/*
	LINK_ANALYSE_INEX_WIKIPEDIA.C
	-----------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../source/disk.h"
#include "../source/file.h"

#define PAGERANK_D ((double)0.85)

class ANT_link;


/*
	class ANT_LINK_NAME_PAIR
	------------------------
*/
class ANT_link_name_pair
{
public:
	long source, destination;
public:
	static int cmp_on_target(const void *a, const void *b);
} ;


/*
	ANT_LINK_NAME_PAIR::CMP_ON_TARGET()
	-----------------------------------
*/
int ANT_link_name_pair::cmp_on_target(const void *a, const void *b)
{
ANT_link_name_pair *one, *two;

one = (ANT_link_name_pair *)a;
two = (ANT_link_name_pair *)b;

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
	class ANT_LINK_NODE
	-------------------
*/
class ANT_link_node
{
public:
	long long name;
	long long outdegree;
	long long indegree;

public:
	static int cmp(const void *a, const void *b);
	static int long_cmp(const void *a, const void *b);
} ;

/*
	ANT_LINK_NODE::CMP()
	--------------------
*/
int ANT_link_node::cmp(const void *a, const void *b)
{
ANT_link_node *one, *two;

one = (ANT_link_node *)a;
two = (ANT_link_node *)b;

return one->name < two->name ? -1 : one->name > two->name ? 1 : 0;
}

/*
	ANT_LINK_NODE::LONG_CMP()
	-------------------------
*/
int ANT_link_node::long_cmp(const void *a, const void *b)
{
long *one;
ANT_link_node *two;

one = (long *)a;
two = (ANT_link_node *)b;

return *one < two->name ? -1 : *one > two->name ? 1 : 0;
}



/*
	class ANT_LINK
	--------------
*/
class ANT_link
{
public:
	ANT_link_node *source, *destination;
} ;

/*
	COUNT_LINKS()
	-------------
	Work out how many links there are in the input files by getting their
	file size and dividing by the size of a link
*/
long long count_links(int argc, char *argv[])
{
long long total_links;
long long param;

total_links = 0;
for (param = 1; param < argc; param++)
	{
	ANT_file file;
	file.open(argv[param], "rb");
	total_links += file.file_length() / (sizeof(long) * 2);
	file.close();
	}
return total_links;
}

/*
	READ_LINK_GRAPH()
	-----------------
	Read all the input files into a single array that holds all the links
*/
void read_link_graph(int argc, char *argv[], ANT_link_name_pair *link_graph)
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
	MAKE_NODES()
	------------
*/
ANT_link *make_nodes(ANT_link_node *nodes, ANT_link_name_pair *graph, long long total_links)
{
long long which_link, previous_node, total_nodes;
ANT_link *new_graph;
ANT_link_node *current_node, *got;
ANT_link_name_pair *current;

puts("Generate Nodes");
current_node = nodes;
previous_node = -1;
for (current = graph; current->destination >= 0; current++)
	if (current->destination != previous_node)
		{
		current_node->name = current->destination;
		current_node->outdegree = 0;
		current_node->indegree = 0;
		previous_node = current->destination;
		current_node++;
		}

total_nodes = current_node - nodes;
current_node->name = current_node->indegree = current_node->outdegree = -1;		// sentinal

puts("Sort Nodes");
qsort(nodes, (size_t)total_nodes, sizeof(*nodes), ANT_link_node::cmp);


puts("Rebuild link graph (and compute indegree and outdegree)");
new_graph = new ANT_link[(size_t)(total_links + 1)];
for (which_link = 0; which_link < total_links; which_link++)
	{
	got = (ANT_link_node *)bsearch(&graph[which_link].source, nodes, (size_t)total_nodes, sizeof(*nodes), ANT_link_node::long_cmp);
	new_graph[which_link].source = got;
	if (got != NULL)
		got->outdegree++;

	got = (ANT_link_node *)bsearch(&graph[which_link].destination, nodes, (size_t)total_nodes, sizeof(*nodes), ANT_link_node::long_cmp);
	new_graph[which_link].destination = got;
	if (got != NULL)
		got->indegree++;
	}

new_graph[which_link].source = new_graph[which_link].destination = NULL;		// sentinal
return new_graph;
}

/*
	WRITE_GRAPH()
	-------------
*/
void write_graph(ANT_link_node *graph, double *pagerank, double **ranked_list)
{
ANT_link_node *current;
long long rank_position, which;

#ifdef NEVER
for (current = graph; current->name >= 0; current++)
	printf("PageRank:%e Node:%lld indegree:%lld outdegree:%lld\n", pagerank[current - graph], (long long)current->name, (long long)current->indegree, (long long)current->outdegree);
#endif


rank_position = 0;
for (current = graph; current->name >= 0; current++)
	{
	which = ranked_list[rank_position] - pagerank;
	printf("PageRank:%e Node:%lld indegree:%lld outdegree:%lld\n", pagerank[which], graph[which].name, graph[which].indegree, graph[which].outdegree);
	rank_position++;
	}

}

/*
	COMPUTE_PAGERANK()
	------------------
*/
double *compute_pagerank(long long iterations, ANT_link_node *nodes, ANT_link *graph)
{
ANT_link *current;
double *source_pagerank, *destination_pagerank, *tmp;
long long source, number_of_nodes, which_node, iteration, destination;
ANT_link_node *current_node;

number_of_nodes = 0;
for (current_node = nodes; current_node->name >= 0; current_node++)
	number_of_nodes++;

source_pagerank = new double[(size_t)number_of_nodes];
destination_pagerank = new double[(size_t)number_of_nodes];

for (which_node = 0; which_node < number_of_nodes; which_node++)
	source_pagerank[which_node] = 1.0 / (double)number_of_nodes;

for (iteration = 0; iteration < iterations; iteration++)
	{
	puts("PageRank iteration");
	memset(destination_pagerank, 0, (size_t)(sizeof(*destination_pagerank) * number_of_nodes));		// set to 0

	for (current = graph; current->source != NULL || current->destination != NULL; current++)
		if (current->source != NULL && current->source->outdegree != 0)
			{
			source = current->source - nodes;
			destination = current->destination - nodes;

			destination_pagerank[destination] += source_pagerank[source] / (double)current->source->outdegree;
			}

	for (which_node = 0; which_node < number_of_nodes; which_node++)
		destination_pagerank[which_node] = ((1.0 - PAGERANK_D) / number_of_nodes) + PAGERANK_D * destination_pagerank[which_node];

	tmp = source_pagerank;
	source_pagerank = destination_pagerank;
	destination_pagerank = tmp;
	}

delete [] destination_pagerank;
return source_pagerank;
}

/*
	DOUBLE_POINTER_CMP()
	--------------------
	return highest at the top, lowest at the bottom
*/
int double_pointer_cmp(const void *a, const void *b)
{
double **one, **two;

one = (double **)a;
two = (double **)b;

return **one < **two ? 1 : **one > **two ? -1 : 0;
}

/*
	SORT_PAGERANKS()
	----------------
*/
double **sort_pageranks(ANT_link_node *nodes, double *pagerank)
{
double **pointers;
long long number_of_nodes, which;
ANT_link_node *current_node;

number_of_nodes = 0;
for (current_node = nodes; current_node->name >= 0; current_node++)
	number_of_nodes++;

pointers = new double *[(size_t)number_of_nodes];
for (which = 0; which < number_of_nodes; which++)
	pointers[which] = pagerank + which;

qsort(pointers, (size_t)number_of_nodes, sizeof(*pointers), double_pointer_cmp);

return pointers;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
long long total_links;
ANT_link_name_pair *link_graph;
ANT_link_node *nodes;
ANT_link *link_graph_as_pointers;
double *pagerank_scores, **ranked_list;

if (argc < 2)
	exit(printf("Usage:%s <infile>...<infile>\n", argv[0]));

puts("Compute the size of the link graph");
total_links = count_links(argc, argv);

puts("Allocate memory for the link graph");
link_graph = new ANT_link_name_pair[(size_t)(total_links + 1)];
nodes = new ANT_link_node[(size_t)(total_links + 1)];				// this is the worst case

puts("Load link graph");
read_link_graph(argc, argv, link_graph);
link_graph[total_links].source = link_graph[total_links].destination = -1;		// sentinal at the end

puts("Sort link graph on indegree");
qsort(link_graph, (size_t)total_links, sizeof(*link_graph), ANT_link_name_pair::cmp_on_target);

puts("Build the graph");
link_graph_as_pointers = make_nodes(nodes, link_graph, total_links);

pagerank_scores = compute_pagerank(3, nodes, link_graph_as_pointers);

puts("Rank on pagerank");
ranked_list = sort_pageranks(nodes, pagerank_scores);

#ifndef NEVER
	puts("Dump graph");
	write_graph(nodes, pagerank_scores, ranked_list);
#endif

return 0;
}

