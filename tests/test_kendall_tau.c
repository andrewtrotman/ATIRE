#include <cstdio>
#include <cstdlib>
#include <utility>

#include "../source/pregen_kendall_tau.h"

/* We use a mixture of different sizes just to help test all the codepaths and find off-by-ones */
#define TEST_1_SIZE 20
#define TEST_2_SIZE 253
#define TEST_3_SIZE 256
#define TEST_4_SIZE 1023
#define TEST_BIG_SIZE 1023125

int approx_equal(double a, double b)
{
return abs(a - b) < 0.0001;
}

int test_check_approx_equal(int testnum, double measured, double expected)
{
int pass = approx_equal(measured, expected);

printf("Test %d %s: expected %f, got: %f\n", testnum, pass ? "pass" : "FAIL", expected, measured);

return pass;
}

int main(void)
{
std::pair<pregen_t, pregen_t> *test1 = new std::pair<pregen_t, pregen_t>[TEST_1_SIZE];
std::pair<pregen_t, pregen_t> *test2 = new std::pair<pregen_t, pregen_t>[TEST_2_SIZE];
std::pair<pregen_t, pregen_t> *test3 = new std::pair<pregen_t, pregen_t>[TEST_3_SIZE];
std::pair<pregen_t, pregen_t> *test4 = new std::pair<pregen_t, pregen_t>[TEST_4_SIZE];
std::pair<pregen_t, pregen_t> *test_big = new std::pair<pregen_t, pregen_t>[TEST_BIG_SIZE];
int works = 1;

/* Identical relative rankings */
for (int i = 0; i < TEST_1_SIZE; i++)
	{
	test1[i].first = i;
	test1[i].second = i * 2 + 1;
	}

works = works && test_check_approx_equal(1, kendall_tau(test1, TEST_1_SIZE), 1.0);

/* Exactly opposite relative rankings */
for (int i = 0; i < TEST_2_SIZE; i++)
	{
	test2[i].first = i;
	test2[i].second = TEST_2_SIZE * 10 - (i * 2 + 1);
	}

works = works && test_check_approx_equal(2, kendall_tau(test2, TEST_2_SIZE), -1.0);

/* Identical relative rankings, including plenty of "ties" in values */
for (int i = 0; i < TEST_3_SIZE; i++)
	{
	test3[i].first = i % 23;
	test3[i].second = (i % 23) * 3 + 10;
	}

works = works && test_check_approx_equal(3, kendall_tau(test3, TEST_3_SIZE), 1.0);

/* Exactly opposite relative rankings, including plenty of ties */
for (int i = 0; i < TEST_4_SIZE; i++)
	{
	test4[i].first = i % 23;
	test4[i].second = TEST_4_SIZE * 10 - ((i % 23) * 3 + 10);
	}

works = works && test_check_approx_equal(4, kendall_tau(test4, TEST_4_SIZE), -1.0);


/* Random rankings which should show very little correlation. Hey, I hope the
 * standard library has a good random number generator! */
for (int i = 0; i < TEST_BIG_SIZE; i++)
	{
	test_big[i].first = rand();
	test_big[i].second = rand();
	}

works = works && test_check_approx_equal(5, kendall_tau(test_big, TEST_BIG_SIZE), 0.0);

delete[] test1;
delete[] test2;
delete[] test3;
delete[] test4;
delete[] test_big;

if (works)
	{
	printf("Tests pass.\n");

	return EXIT_SUCCESS;
	}
else
	{
	printf("Tests fail.\n");

	return EXIT_FAILURE;
	}
}
