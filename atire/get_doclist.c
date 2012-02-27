/*
	GET_DOCLIST.C
	-------------
*/
#include "../source/file.h"
#include "../source/memory.h"
#include "../source/search_engine.h"
#include "../source/str.h"

/*
	MAIN()
	------
*/
int main(void)
{
ANT_memory memory;
ANT_search_engine search_engine(&memory);
ANT_file doclist;
unsigned long buffer_length;
long long start, end, doc;
char *buffer;
char **filenames;

search_engine.open();

/*
	Don't overwrite doclist.aspt if it exists, which we check by opening read-only first
*/
if (doclist.open("doclist.aspt", "r"))
	exit(printf("doclist.aspt already exists\n"));
doclist.close();
doclist.open("doclist.aspt", "w");

start = search_engine.get_variable("~documentfilenamesstart");
end = search_engine.get_variable("~documentfilenamesfinish");

if (start == 0 || end == 0)
	exit(printf("Filenames aren't in the index\n"));

buffer_length = (unsigned long)(end - start);
buffer = (char *)malloc(sizeof(*buffer) * buffer_length);

filenames = search_engine.get_document_filenames(buffer, &buffer_length);

for (doc = 0; doc < search_engine.document_count(); doc++)
	doclist.puts(strip_space_inplace(filenames[doc]));

doclist.close();

return 0;
}
