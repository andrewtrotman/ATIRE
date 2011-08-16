#include <stdlib.h>
#include <stdio.h>

#include "../source/str.h"
#include "../source/string_pair.h"

extern "C"
{
#include "mysql.h"
}

/* How many rows to fetch at once from MySQL */
#define MYSQL_FETCH_PAGESIZE 10000
#define MYSQL_RANGE_START_MARKER "$start"
#define MAX_LONG_LONG_FORMATTED_LEN 20

/* Different modes for fetching rows from MySQL */
enum ANT_mysql_paging_mode
{
	FETCH_ALL, /* Fetch everything in one call (requires local memory copy of entire dataset) */
	FETCH_LIMIT, /* Fetch in pages by adding an LIMIT x,y clause to the end of the query (LIMIT x,y is slow for large x). Useful if rows have no numeric primary key */
	FETCH_RANGE_START_LIMIT /* Fetch in pages by modifying a variable $start in the WHERE clause to seek on row id numbers, and adding a LIMIT clause. Best option! Requires first column selected to be numeric primary key. */
};

MYSQL the_connection, *connection;
ANT_string_pair query, query_mod;
MYSQL_RES *result;
char *page_start_marker_pos;
long long page_start, page_remain;

int query_limit()
{
sprintf((char*)query_mod.start, "%s LIMIT %lld,%lld", query.start, (long long) page_start, (long long) page_remain);

mysql_query(connection, (char*)query_mod.start);

if ((result = mysql_store_result(connection)) == NULL)
	return 0;

return 1;
}

int query_range_start_limit()
{
sprintf((char*)query_mod.start, "%s%lld%s LIMIT %lld", (char*)query.start, (long long) page_start, page_start_marker_pos + strlen(MYSQL_RANGE_START_MARKER), (long long) page_remain);

mysql_query(connection, (char*)query_mod.start);

if ((result = mysql_store_result(connection)) == NULL)
	return 0;

return 1;
}


int main(int argc, char * argv[])
{
long fields;
MYSQL_ROW row;
unsigned long *field_length;
int docname_col;

long long rows_dumped = 0;
long long next_page_start = 0;
ANT_mysql_paging_mode mode;

if (argc < 6)
	{
	fprintf(stderr, "Usage: <username> <password> <hostname> <database> <query>\n");
	exit(EXIT_FAILURE);
	}

connection = mysql_init(&the_connection);
mysql_real_connect(connection, argv[3], argv[1], argv[2], argv[4], 0, NULL, CLIENT_COMPRESS);

query.start = strnew(argv[5]);
query.string_length = strlen(query.start);

query_mod.start = NULL;

mode = FETCH_RANGE_START_LIMIT;

switch (mode)
	{
	case FETCH_ALL:
		page_start = 0;
		page_remain = 0;

		mysql_query(connection, (char*)query.start);

		if ((result = mysql_store_result(connection)) == NULL)
			return EXIT_FAILURE;

		docname_col = 0;
	break;
	case FETCH_LIMIT:
		/* We want enough space to store a modified query that includes a limit clause */
		query_mod.string_length = query.string_length + strlen(" LIMIT ") + MAX_LONG_LONG_FORMATTED_LEN + strlen(",") + MAX_LONG_LONG_FORMATTED_LEN;
		/* For good luck, I am going to add a bit extra on */
		query_mod.string_length += 10;

		query_mod.start = new char[query_mod.string_length + 1];

		page_start = 0;
		page_remain = MYSQL_FETCH_PAGESIZE;

		if (!query_limit())
			return EXIT_FAILURE;

		docname_col = 0;
	break;
	case FETCH_RANGE_START_LIMIT:
		/* We want enough space to store a modified query that uses a WHERE clause with one value
		 * and a LIMIT clause to page through the dataset.
		 *
		 * We'll be replacing the string MYSQL_RANGE_START_MARKER in the original query with
		 * a long long. */
		query_mod.string_length = query.string_length - strlen(MYSQL_RANGE_START_MARKER) + MAX_LONG_LONG_FORMATTED_LEN + strlen(" LIMIT ") + MAX_LONG_LONG_FORMATTED_LEN;
		/* For good luck, I am going to add a bit extra on */
		query_mod.string_length += 10;

		query_mod.start = new char[query_mod.string_length + 1];

		//Mark the start of the range marker to be replaced
		page_start_marker_pos = strstr(query.start, MYSQL_RANGE_START_MARKER);
		if (!page_start_marker_pos)
			{
			fprintf(stderr, "Missing range start marker in query: %s\n", MYSQL_RANGE_START_MARKER);
			exit(EXIT_FAILURE);
			}
		*page_start_marker_pos = '\0';

		page_start = -1;
		page_remain = MYSQL_FETCH_PAGESIZE;

		if (!query_range_start_limit())
			return EXIT_FAILURE;

		docname_col = 1;
	break;
	default:
		fprintf(stderr, "Unknown MySQL fetch method\n");
		exit(-1);
	}

fields = mysql_num_fields(result);

while (1)
	{
	while ((row = mysql_fetch_row(result)) == NULL)
		{
		mysql_free_result(result);

		/* If there could have been another row, but there wasn't, we're at the end of the dataset */
		if (mode == FETCH_ALL || page_remain > 0)
			{
			fprintf(stderr, "Successfully dumped %lld rows.\n", rows_dumped);
			return EXIT_SUCCESS;
			}

		/* Otherwise, fetch the next page */
		switch (mode)
			{
			case FETCH_LIMIT:
				page_start = page_start + MYSQL_FETCH_PAGESIZE;
				page_remain = MYSQL_FETCH_PAGESIZE;

				query_limit();
				break;
			case FETCH_RANGE_START_LIMIT:
				page_start = next_page_start;
				page_remain = MYSQL_FETCH_PAGESIZE;

				query_range_start_limit();
				break;
			default:
				fprintf(stderr, "Unknown MySQL fetch method.\n");
				exit(-1);
			}
		}

	switch (mode)
		{
		case FETCH_LIMIT:
			page_remain--;
			break;
		case FETCH_RANGE_START_LIMIT:
			page_remain--;
			//Is this the last row in the set? Remember the row's id so that we can make our next ranged query
			if (page_remain == 0)
				next_page_start = strtol(row[0], NULL, 10);

			//First field is our range start, so doc name is second
			break;
		default:
			break;
		}

	field_length = mysql_fetch_lengths(result);

	for (long current = 0; current < fields; current++)
		if (row[current] != NULL)
			printf("%.*s ", (int) field_length[current], row[current]);
	printf("\n");

	rows_dumped++;
	}
}
