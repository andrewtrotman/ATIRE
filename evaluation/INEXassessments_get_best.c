/*
	INEXASSESSMENTS_GET_BEST.C
	--------------------------
	Walk through and INEX 2008 assessment file and remove all assessments that are not
	as good as the most relevant document - where most relevant is the highest proportion
	of the document being relecant (relbytes/doclength)
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../source/disk.h"

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
char *file;
char **list, **current, **from;
long long length;
long topic, document, relevant_characters, document_length, last_topic;
double best, relevant;
double within;

if (argc != 3)
	exit(printf("Usage:%s <qrelfile> <within_percent>\n", argv[0]));

within = atof(argv[2]) / 100.0;

if ((file = ANT_disk::read_entire_file(argv[1])) == NULL)
	exit(printf("Cannot read input file:%s\n", argv[1]));

list = ANT_disk::buffer_to_list(file, &length);

last_topic = -1;
for (from = current = list; *current != NULL; current++)
	{
	sscanf(*current, "%ld %*s %ld %ld %ld", &topic, &document, &relevant_characters, &document_length);
	if (topic != last_topic)
		{
		while (from < current)
			{
			sscanf(*from, "%ld %*s %ld %ld %ld", &topic, &document, &relevant_characters, &document_length);
			if (relevant_characters > document_length)
				relevant_characters = document_length;
			relevant = (double)relevant_characters / (double)document_length;
			if (fabs(relevant - best) <= within)
				{
//				printf("%f %f %f %f\n", best, relevant, fabs(relevant - best), within);
				printf("%ld Q0 %ld %ld %ld\n", topic, document, relevant_characters, document_length);
				}
			from++;
			}

		sscanf(*current, "%ld %*s %ld %ld %ld", &topic, &document, &relevant_characters, &document_length);
		from = current;
		best = 0.0;
		last_topic = topic;
		}
	if (relevant_characters > document_length)
		relevant_characters = document_length;
	relevant = (double)relevant_characters / (double)document_length;
	if (relevant > best)
		best = relevant;
	}

while (from < current)
	{
	sscanf(*from, "%ld %*s %ld %ld %ld", &topic, &document, &relevant_characters, &document_length);
	if (relevant_characters > document_length)
		relevant_characters = document_length;
	relevant = (double)relevant_characters / (double)document_length;
	if (relevant >= best)
		printf("%ld Q0 %ld %ld %ld\n", topic, document, relevant_characters, document_length);
	from++;
	}

return 0;
}