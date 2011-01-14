/*
	TEARA_RUN_TO_TREC.C
	-------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

static const long long HIGHEST_RESULT (1024 * 1024);
static char true_run_name[1024 * 1024];
static char buffer[1024 * 1024];

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
char *run_name, *end_run_name;
FILE *fp;
long long topic, destination, result_pos, result, position;
long long line, had_beps;

if (argc != 2)
	exit(printf("usage:%s <INEX_2010_LTW_run_file>", argv[0]));

if ((fp = fopen(argv[1], "rb")) == NULL)
	exit(printf("Can't open infile:%s\n", argv[1]));

line = 0;
while (fgets(buffer, sizeof(buffer), fp) != NULL)
	{
	line++;

	if ((run_name = strstr(buffer, " run-id=")) != NULL)
		{
		run_name = strchr(run_name, '=') + 2;
		end_run_name = strchr(run_name, '"');
		sprintf(true_run_name, "%*.*s", end_run_name - run_name, end_run_name - run_name, run_name);
		}
	else if (strstr(buffer, "<topic") != NULL)
		{
		topic = _atoi64(strpbrk(buffer, "1234567890"));
		result = 1;
		position = 1;
		had_beps = false;
		}
	else if (strstr(buffer, "<anchor") != NULL)
		{
		if (had_beps)			// some runs (QUT's runs) have <anchor> tags without <tobep> tags!
			result++;
		}
	else if (strstr(buffer, "<tobep") != NULL)
		{
		had_beps = true;
		position++;
		destination = _atoi64(strchr(buffer, '>') + 1);
		if (topic < 0)
			printf("Error line %lld: topic id not set\n", line);
		else
			printf("%lld Q0 %lld %lld %lld %s\n", topic, destination, result, HIGHEST_RESULT - position, true_run_name);
		}
	}
}

