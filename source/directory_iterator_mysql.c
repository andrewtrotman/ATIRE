/*
	DIRECTORY_ITERATOR_MYSQL.C
	--------------------------
*/

#ifdef ANT_HAS_MYSQL

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
mysql_real_connect(connection, server, username, password, database, 0, NULL, CLIENT_COMPRESS);
this->query = strnew(query);
}

/*
	ANT_DIRECTORY_ITERATOR_MYSQL::~ANT_DIRECTORY_ITERATOR_MYSQL()
	-------------------------------------------------------------
*/
ANT_directory_iterator_mysql::~ANT_directory_iterator_mysql()
{
delete [] query;
mysql_close(connection);
}

/*
	ANT_DIRECTORY_ITERATOR_MYSQL::FIRST()
	-------------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_mysql::first(ANT_directory_iterator_object *object)
{
mysql_query(connection, query);

if ((result = mysql_store_result(connection)) == NULL)
	return NULL;

fields = mysql_num_fields(result);

return next(object);
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
char *into;

if ((row = mysql_fetch_row(result)) == NULL)
	{
	mysql_free_result(result);
	return NULL;
	}

field_length = mysql_fetch_lengths(result);

object->filename = strnew(row[0]);

if (get_file)
	{
	total_length = 0;
	for(current = 0; current < fields; current++)
		total_length += field_length[current] + 1;

	into = object->file = new char [object->length = total_length += 1];

	for(current = 0; current < fields; current++)
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

