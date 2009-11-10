/*
	GET_DOCLIST.C
	-------------
*/
#include "../source/memory.h"
#include "../source/search_engine.h"
#include "../source/search_engine_btree_leaf.h"

/*
	GET_VARIABLE()
	--------------
*/
long long get_variable(ANT_search_engine *engine, char *name)
{
return engine->get_variable(name);
}

/*
	MAIN()
	------
*/
int main(void)
{
ANT_memory memory;
ANT_search_engine search_engine(&memory);
long long start, end;

start = get_variable(&search_engine, "~documentfilenamesstart");
end = get_variable(&search_engine, "~documentfilenamesfinish");
printf("start:%lld end:%lld\n", start, end);

return 0;
}
