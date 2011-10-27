/*
	DIRECTORY_ITERATOR_MYSQL.C
	--------------------------
*/

#ifdef ANT_HAS_MYSQL

#include <stdlib.h>
#include <stdio.h>

#include "directory_iterator_mysql.h"
#include "str.h"

/*
	ANT_DIRECTORY_ITERATOR_MYSQL::ANT_DIRECTORY_ITERATOR_MYSQL()
	------------------------------------------------------------
*/
ANT_directory_iterator_mysql::ANT_directory_iterator_mysql(char *server, char *username, char *password, char *database, char *query, long get_file) : ANT_directory_iterator("", get_file)
{
connection = mysql_init(&the_connection);

if (!mysql_real_connect(connection, server, username, password, database, 0, NULL, CLIENT_COMPRESS))
	 fprintf(stderr, "MySQL failed to connect: %s\n", mysql_error(connection));

/*
	We want utf-8 encoded text from MySQL (not some 1-byte Windows encoding that
	replaces most characters with ?)
 */
mysql_query(connection, "SET NAMES 'utf8'");

this->query.start = strnew(query);
this->query.string_length = strlen(query);

this->query_mod.start = NULL;

this->mode = FETCH_ALL;
}

/*
	ANT_DIRECTORY_ITERATOR_MYSQL::~ANT_DIRECTORY_ITERATOR_MYSQL()
	-------------------------------------------------------------
*/
ANT_directory_iterator_mysql::~ANT_directory_iterator_mysql()
{
delete [] query.start;
delete [] query_mod.start;

mysql_close(connection);
}

int ANT_directory_iterator_mysql::query_limit(long long page_start, long long page_size)
{
this->page_start = page_start;
this->page_remain = page_size;

sprintf((char*)query_mod.start, "%s LIMIT %lld,%lld", query.start, (long long) this->page_start, (long long) this->page_remain);

//printf("Query: %s\n", query_mod.start);
mysql_query(connection, (char*)query_mod.start);

if ((result = mysql_use_result(connection)) == NULL)
	return 0;

fields = mysql_num_fields(result);

return 1;
}

int ANT_directory_iterator_mysql::query_range_start_limit(long long page_start, long long page_size)
{
this->page_start = page_start;
this->page_remain = page_size;

sprintf((char*)query_mod.start, "%s%lld%s LIMIT %lld", (char*)query.start, (long long) this->page_start, page_start_marker_pos + strlen(MYSQL_RANGE_START_MARKER), (long long) this->page_remain);

//printf("Query: %s\n", query_mod.start);
mysql_query(connection, (char*)query_mod.start);

if ((result = mysql_use_result(connection)) == NULL)
	return 0;

fields = mysql_num_fields(result);

return 1;
}

/*
	ANT_DIRECTORY_ITERATOR_MYSQL::FIRST()
	-------------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_mysql::first(ANT_directory_iterator_object *object)
{
switch (mode)
{
	case FETCH_ALL:
		mysql_query(connection, (char*)query.start);

		page_remain = 0;

		if ((result = mysql_use_result(connection)) == NULL)
			return NULL;

		fields = mysql_num_fields(result);
	break;
	case FETCH_LIMIT:
		/* We want enough space to store a modified query that includes a limit clause */
		query_mod.string_length = query.string_length + strlen(" LIMIT ") + MAX_LONG_LONG_FORMATTED_LEN + strlen(",") + MAX_LONG_LONG_FORMATTED_LEN;
		/* For good luck, I am going to add a bit extra on */
		query_mod.string_length += 10;

		query_mod.start = new char[query_mod.string_length + 1];

		if (!query_limit(0, MYSQL_FETCH_PAGESIZE))
			return NULL;
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
		*page_start_marker_pos = '\0';

		if (!query_range_start_limit(-1, MYSQL_FETCH_PAGESIZE))
			return NULL;
	break;
	default:
		fprintf(stderr, "Unknown MySQL fetch method\n");
		exit(-1);
}

return next(object);
}

void ANT_directory_iterator_mysql::set_paging_mode(ANT_mysql_paging_mode mode)
{
this->mode = mode;
}

/*
	ANT_DIRECTORY_ITERATOR_MYSQL::NEXT()
	------------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_mysql::next(ANT_directory_iterator_object *object)
{
MYSQL_ROW row;
unsigned long *field_length, total_length;
long current;
char *into, *pos;
int docname_col;

while ((row = mysql_fetch_row(result)) == NULL)
	{
	mysql_free_result(result);

	/* If there could have been another row, but there wasn't, we're at the end of the dataset */
	if (mode == FETCH_ALL || page_remain > 0)
		return NULL;

	/* Otherwise, fetch the next page */
	switch (mode)
		{
		case FETCH_LIMIT:
			query_limit(page_start + MYSQL_FETCH_PAGESIZE, MYSQL_FETCH_PAGESIZE);
			break;
		case FETCH_RANGE_START_LIMIT:
			query_range_start_limit(next_page_start, MYSQL_FETCH_PAGESIZE);
			break;
		default:
			fprintf(stderr, "Unknown MySQL fetch method.\n");
			exit(-1);
		}
	}

field_length = mysql_fetch_lengths(result);

switch (mode)
	{
	case FETCH_LIMIT:
		page_remain--;

		docname_col = 0;
		break;
	case FETCH_RANGE_START_LIMIT:
		page_remain--;
		//Is this the last row in the set? Remember the row's id so that we can make our next ranged query
		if (page_remain == 0)
			next_page_start = strtol(row[0], NULL, 10);

		//First field is our range start, so doc name is second
		docname_col = 1;
		break;
	default:
		docname_col = 0;
	}

if (docname_col < fields && row[docname_col])
	{
	object->filename = strnew(row[docname_col]);

	/* Document names must not contain newlines, since doclist files are separated by
	 * newlines.
	 */
	for (pos = object->filename; *pos != 0; ++pos)
		if (*pos == '\n')
			*pos = ' ';
	}
else
	object->filename = strnew("");

if (get_file)
	{
	total_length = 0;
	for(current = docname_col + 1; current < fields; current++)
		total_length += field_length[current] + 1;

	object->length = total_length + 1;
	into = object->file = new char [(size_t) object->length];

	for(current = docname_col + 1; current < fields; current++)
		{
		if (row[current] != NULL)
			{
			strncpy(into, row[current], field_length[current]);
			into += field_length[current];
			}
		*into++ = ' ';
		}
	*into = '\0';
	}
else
	{
	object->length = 0;
	object->file = NULL;
	}

return object;
}

#endif

