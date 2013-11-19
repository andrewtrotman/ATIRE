/*
	MAP_FAKE.C
	----------
	compute the best, worst, and mean precision you'll get if you
	had all possible permutations of DocRel relevant documents and
	DocNotRel non-relevant documents in a results list.  This was 
	written to get a measure on human performance.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ITERATIONS 10000

long satisfied[128];

static long result_list[1024 * 1024 * 16];

/*
	SHUFFLE()
	---------
	Durstenfeld's algorithm from the Wikipedia
*/
void shuffle(long *cards, long n)
{
long k, tmp;

while (n > 1)
	{
	k = rand() % n;
	n--;
	tmp = cards[n];
	cards[n] = cards[k];
	cards[k] = tmp;
	}
}

/*
	COMPUTE_AVP()
	-------------
*/
double compute_avp(long *result_list, long length, long total_relevant)
{
long times;
long found_and_relevant = 0;
double precision = 0;

for (times = 0; times < length; times++)
	{
	if (result_list[times] != 0)
		{
		found_and_relevant++;
		precision += (double)found_and_relevant / (double)(times + 1);
		}
	}

return precision / total_relevant;
}

/*
	COMPUTE_SATISFACTION()
	----------------------
*/
void compute_satisfaction(long *results_list, long length)
{
long which, found;

found = 0;
for (which = 0; which < (length < 10 ? length : 10); which++)
	{
	if (results_list[which] != 0)
		found = 1;
	satisfied[which] += found;
	}

while (which < 10)
	{
	satisfied[which] += found;
	which++;
	}
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
long which, times;
long topic, relevant, non_relevant, total_relevant;
double precision;

if (argc != 5)
	exit(printf("%s <topic> <DocsRel> <DocsNotRel> <TotalRel>\n", argv[0]));

topic = atol(argv[1]);
relevant = atol(argv[2]);
non_relevant = atol(argv[3]);
total_relevant = atol(argv[4]);

/*
	Topic number
*/
printf("%ld ", topic);

/*
	Compute the best score
*/
precision = (double)relevant / (double)total_relevant;
printf("%f ", precision);

/*
	Compute the worst score
*/
precision = 0;
for (which = 1; which <= relevant; which++)
	precision += (double)which / (double)(non_relevant + which);
precision /= total_relevant;
printf("%f ", precision);

/*
	Now generate some permutations and compute the average precision
*/
memset(satisfied, 0, sizeof(satisfied));
for (times = 0; times < ITERATIONS; times++)
	{
	memset(result_list, 0, sizeof(*result_list) * (relevant + non_relevant));		// all non-relevant
	for (which = 0; which < relevant; which++)
		result_list[which] = 1;							// put relevant at the top
	shuffle(result_list, relevant + non_relevant);
	precision += compute_avp(result_list, relevant + non_relevant, total_relevant);

	/*
		Compute the satisfaction levels (has relevant in top-n)
	*/
	compute_satisfaction(result_list, relevant + non_relevant);
	}
precision /= (double)ITERATIONS;

printf("%f\n", precision);
printf("SATISFACTION:");
for (times = 0; times < 10; times++)
	printf("%f ", (double)satisfied[times] / (double)ITERATIONS);
printf("\n");

return 0;
}
