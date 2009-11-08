/*
	GET_DOCLIST.C
	-------------
*/
#include "../source/memory.h"
#include "../source/search_engine.h"
#include "../source/search_engine_btree_leaf.h"

/*
	MAIN()
	------
*/
int main(void)
{
ANT_memory memory;
ANT_search_engine search_engine(&memory);
long long start, end;

start = search_engine.get_variable("~documentfilenamesstart");
end = search_engine.get_variable("~documentfilenamesfinish");

printf("start:%lld end:%lld\n", start, end);
return 0;
}
