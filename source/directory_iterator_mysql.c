#ifdef ANT_HAS_MYSQL

	#include "my_global.h"
	#include "mysql.h"

	/*
		MAIN()
		------
	*/
	int main(int argc, char **argv)
	{
	MYSQL *conn, connection;
	MYSQL_RES *result;
	MYSQL_ROW row;
	MYSQL_FIELD *field;
	int num_fields;
	int i;

	conn = mysql_init(&connection);
	mysql_real_connect(conn, "localhost", "root", "qwerty6!", "test", 0, NULL, CLIENT_COMPRESS);

	mysql_query(conn, "SELECT * FROM testtable");
	result = mysql_store_result(conn);

	num_fields = mysql_num_fields(result);

	while(field = mysql_fetch_field(result))
		printf("%s ", field->name);
	printf("\n");

	while ((row = mysql_fetch_row(result)))
		{
		for(i = 0; i < num_fields; i++)
			printf("%s  ", row[i] ? row[i] : "NULL");
		printf("\n");
		}

	mysql_free_result(result);
	mysql_close(conn);
	}

#endif

