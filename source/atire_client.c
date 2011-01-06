/*
	ATIRE_CLIENT.C
	--------------
*/
#include <stdlib.h>
#include <stdio.h>
#include "ATIRE_API_remote.h"

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ATIRE_API_remote server;
char *got;

if (argc != 2)
	exit(printf("Usage: %s <server:port>\n", argv[0]));

if (!server.open(argv[1]))
	exit(printf("Cannot connect to:%s\n", argv[1]));

puts(got = server.search("trotman", 1, 11));
delete [] got;

puts("");
puts("----");
puts(got = server.get_document(149436));
delete [] got;
puts("----");
puts("");

puts(got = server.search("andrew trotman", 1, 10));
delete [] got;

puts("");
puts("----");
puts(got = server.get_document(149436));
delete [] got;
puts("----");
puts("");


return 0;
}
