/*
	PREGEN_KENDALL_TAU.C
	--------------------
*/
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <utility>
#include <algorithm>
#include <cmath>

#include "pregen_kendall_tau.h"

/*
	ANT_COMPARE_TAU_P1P2()
	----------------------
*/
static bool ANT_compare_tau_p1p2(const std::pair<ANT_pregen_t, ANT_pregen_t> &a, const std::pair<ANT_pregen_t, ANT_pregen_t> &b)
{
/*
	Sort first on pregen1's score
*/
if (a.first > b.first)
	return true;
if (a.first < b.first)
	return false;
/*
	Break ties using pregen2's score
*/
if (a.second > b.second)
	return true;
if (a.second < b.second)
	return false;

return false;
}

/*
	COMPARE_TAU_P2()
	----------------
*/
static bool ANT_compare_tau_p2(const std::pair<ANT_pregen_t, ANT_pregen_t> &a, const std::pair<ANT_pregen_t, ANT_pregen_t> &b)
{
return a.second > b.second;
}

/*
	ANT_TAU_MERGE()
	---------------
*/
static long long ANT_tau_merge(std::pair<ANT_pregen_t, ANT_pregen_t> *list1, unsigned long long list1count, std::pair<ANT_pregen_t, ANT_pregen_t> *list2, unsigned long long list2count, std::pair<ANT_pregen_t, ANT_pregen_t> *mergebuf)
{
int i = 0, j = 0, m = 0;
long long nswaps = 0;

while (i < list1count || j < list2count)
	if (i >= list1count || (j < list2count && ANT_compare_tau_p2(list2[j], list1[i])))
		{
		nswaps += list1count - i;
		mergebuf[m++] = list2[j++];
		}
	else
		mergebuf[m++] = list1[i++];

return nswaps;
}

/*
	ANT_TAU_MERGE_SORT_INNER()
	--------------------------
*/
static long long ANT_tau_merge_sort_inner(std::pair<ANT_pregen_t, ANT_pregen_t> *docs, unsigned long long doccount, std::pair<ANT_pregen_t, ANT_pregen_t> *mergebuf)
{
long long nswaps = 0;
unsigned long long middle;

if (doccount <= 1)
	return 0;

middle = doccount / 2;

nswaps += ANT_tau_merge_sort_inner(docs, middle, mergebuf);
nswaps += ANT_tau_merge_sort_inner(docs + middle, doccount - middle, mergebuf);

nswaps += ANT_tau_merge(docs, middle, docs + middle, doccount - middle, mergebuf);

for (int i = 0; i < doccount; i++)
	docs[i] = mergebuf[i];

return nswaps;
}

/*
	ANT_TAU_MERGE_SORT()
	--------------------
	Sort the docs list by the values in docs.second.second (i.e., scores from the second pregen).
	Return the number of swaps that bubblesort would have made to sort the same list.
*/
static long long ANT_tau_merge_sort(std::pair<ANT_pregen_t, ANT_pregen_t> *docs, unsigned long long doccount)
{
//TODO shuffling around array indexes would be much faster than sorting pairs of 64-bit values.

std::pair<ANT_pregen_t, ANT_pregen_t> *mergebuf = new std::pair<ANT_pregen_t, ANT_pregen_t>[(size_t) doccount];

long long result = ANT_tau_merge_sort_inner(docs, doccount, mergebuf);

delete [] mergebuf;

return result;
}

/*
	ANT_KENDALL_TAU()
	-----------------
*/
double ANT_kendall_tau(std::pair<ANT_pregen_t, ANT_pregen_t> *docs, int doc_count)
{
//O(n log n) computation of Kendall's Tau from http://www.jstor.org/stable/2282833

//Sort to give an array of (docid, score1, score2) ordered by [score1, score2]
std::sort(docs, docs + doc_count, ANT_compare_tau_p1p2);

long long twicen0 = (long long) doc_count * (doc_count - 1); //List size correction factor
long long twicen1 =  0, twicen2 = 0, twicen3 = 0; //Tie correction factors
long long i, j;

//Find all the groups of values in pregen 1, count how many documents belong to each group
i = 0;
while (i < doc_count - 1)
	{
	unsigned long long ties = 1;

	for (j = i + 1; j < doc_count && docs[i].first == docs[j].first; j++)
		ties++;

	if (ties > 1)
		twicen1 += ties * (ties - 1);

	//Move to next group
	i = j;
	}

unsigned long long swaps = ANT_tau_merge_sort(docs, doc_count);

//Now that the list is sorted by pregen2, we can compute the tie correction factor for groups in pregen2:
i = 0;
while (i < doc_count - 1)
	{
	unsigned long long ties = 1;

	for (j = i + 1; j < doc_count && docs[i].second == docs[j].second; j++)
		ties++;

	if (ties > 1)
		twicen2 += ties * (ties - 1);

	//Move to next group
	i = j;
	}

/*
	Now we're supposed to count "joinly-tied pairs", whatever that means (it's never defined).
	I'm assuming that means	counting blocks where pregen1 and pregen2 are both ties at the same
	time.
 */
i = 0;
while (i < doc_count - 1)
	{
	unsigned long long ties = 1;

	for (j = i + 1; j < doc_count && docs[i].first == docs[j].first && docs[i].second == docs[j].second; j++)
		ties++;

	if (ties > 1)
		twicen3 += ties * (ties - 1);

	//Move to next group
	i = j;
	}

/*
printf("n0 n1 n2 n3 swaps\n");
printf("%lld %lld %lld %lld %lld\n", twicen0/2, twicen1/2, twicen2/2, twicen3/2, swaps);
*/

long long numerator = (twicen0 - twicen1 - twicen2 + twicen3) / 2 - 2 * swaps;
double denominator = sqrt(((double) (twicen0 - twicen1) * (twicen0 - twicen2)) / 4);

return numerator / denominator;
}
