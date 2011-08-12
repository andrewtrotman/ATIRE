/*
	DIRECTORY_ITERATOR_MYSQL.H
	--------------------------
*/
#ifndef DIRECTORY_ITERATOR_MYSQL_H_
#define DIRECTORY_ITERATOR_MYSQL_H_

#ifdef ANT_HAS_MYSQL

#include "string_pair.h"
#include "directory_iterator.h"

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

/*
	class ANT_DIRECTORY_ITERATOR_MYSQL
	----------------------------------
*/
class ANT_directory_iterator_mysql : public ANT_directory_iterator
{
private:
	MYSQL the_connection, *connection;
	MYSQL_RES *result;
	ANT_string_pair query, query_mod;
	long fields;

	char *page_start_marker_pos;

	long long page_start, next_page_start, page_remain;
	ANT_mysql_paging_mode mode;

	int query_limit(long long page_start, long long page_size);
	int query_range_start_limit(long long page_start, long long page_size);
public:
	ANT_directory_iterator_mysql(char *server, char *username, char *password, char *database, char *query, long get_file = 0);
	virtual ~ANT_directory_iterator_mysql();

	virtual void set_paging_mode(ANT_mysql_paging_mode mode);

	virtual ANT_directory_iterator_object *first(ANT_directory_iterator_object *object);
	virtual ANT_directory_iterator_object *next(ANT_directory_iterator_object *object);
} ;

#endif

#endif /* DIRECTORY_ITERATOR_MYSQL_H_ */
