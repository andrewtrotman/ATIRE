/*
	DIRECTORY_ITERATOR_MYSQL.H
	--------------------------
*/
#ifndef DIRECTORY_ITERATOR_MYSQL_H_
#define DIRECTORY_ITERATOR_MYSQL_H_

#ifdef ANT_HAS_MYSQL

	#include "directory_iterator.h"
	extern "C"
	{
	#include "mysql.h"
	}

	/*
		class ANT_DIRECTORY_ITERATOR_MYSQL
		----------------------------------
	*/
	class ANT_directory_iterator_mysql : public ANT_directory_iterator
	{
	private:
		MYSQL the_connection, *connection;
		MYSQL_RES *result;
		char *query;
		long fields;

	public:
		ANT_directory_iterator_mysql(char *server, char *username, char *password, char *database, char *query, long get_file = 0);
		virtual ~ANT_directory_iterator_mysql();

		virtual ANT_directory_iterator_object *first(ANT_directory_iterator_object *object);
		virtual ANT_directory_iterator_object *next(ANT_directory_iterator_object *object);
	} ;

#endif

#endif /* DIRECTORY_ITERATOR_MYSQL_H_ */
