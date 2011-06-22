/*
	ATIRE_BROKE_ENGINE.C
	--------------------
*/
#include <stdio.h>
#include "str.h"
#include "atire_broke_engine.h"
#include "atire_api_remote.h"

/*
	ATIRE_BROKE_ENGINE::ATIRE_BROKE_ENGINE()
	----------------------------------------
*/
ATIRE_broke_engine::ATIRE_broke_engine(char *connect_string)
{
this->connect_string = strnew(connect_string);
server = new ATIRE_API_remote;
server->open(this->connect_string);
}

/*
	ATIRE_BROKE_ENGINE::~ATIRE_BROKE_ENGINE()
	-----------------------------------------
*/
ATIRE_broke_engine::~ATIRE_broke_engine()
{
server->close();
delete server;
}

/*
	ATIRE_BROKE_ENGINE::SEARCH()
	----------------------------
*/
long ATIRE_broke_engine::search(char *query, long long top_of_page, long long page_length)
{
server->search(query, top_of_page, page_length);
return 1;
}
