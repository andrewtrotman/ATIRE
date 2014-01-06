/*
	ATIRE_DOCLIST.C
	---------------
	Dump the list of document "filenames" in the order in which they lay in the index
*/

#include "../source/file.h"
#include "../source/memory.h"
#include "../source/search_engine.h"
#include "../source/str.h"
#include "../source/version.h"

/*
	USAGE()
	-------
*/
int usage(void)
{
puts("GENERAL");
puts("-------");
puts("-? -h -H            Display this help message");
puts("-nologo             Suppress banner (and tail)");
puts("");
puts("FILE HANDLING");
puts("-------------");
puts("-findex <index>     Filename of index (input file) [default=index.aspt]");
puts("-fdoclist <doclist> Filename of doclist (output file) [default=doclist.aspt]");
puts("-noclobber          Do not run if file <doclist> already exists");
return 1;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
char *index_aspt = "index.aspt";
char *doclist_aspt = "doclist.aspt";
ANT_memory memory;
ANT_search_engine search_engine(&memory);
ANT_file doclist;
unsigned long buffer_length;
long long start, end, doc;
char *buffer;
char **filenames;
long current, silent = false, clobber = true;

for (current = 1; current < argc; current++)
	if (strcmp(argv[current], "-findex") == 0)
		index_aspt = argv[++current];
	else if (strcmp(argv[current], "-fdoclist") == 0)
		doclist_aspt = argv[++current];
	else if (strcmp(argv[current], "-nologo") == 0)
		silent = true;
	else if (strcmp(argv[current], "-noclobber") == 0)
		clobber = false;
	else if (strcmp(argv[current], "-?") == 0)
		exit((usage(), 0));
	else if (strcmp(argv[current], "-h") == 0)
		exit((usage(), 0));
	else if (strcmp(argv[current], "-H") == 0)
		exit((usage(), 0));
	else
		exit(usage());

if (!silent)
	puts(ANT_version_string);

if (!search_engine.open(index_aspt))
	exit(printf("Can't open index file %s\n", index_aspt));

if (!clobber)
	{
	if (doclist.open(doclist_aspt, "r"))
		exit(printf("%s already exists\n", doclist_aspt));
	doclist.close();
	}

doclist.open(doclist_aspt, "w");

start = search_engine.get_variable("~documentfilenamesstart");
end = search_engine.get_variable("~documentfilenamesfinish");

if (start == 0 || end == 0)
	exit(printf("Document IDs aren't in the index\n"));

buffer_length = (unsigned long)(end - start);
buffer = (char *)malloc(sizeof(*buffer) * buffer_length);

filenames = search_engine.get_document_filenames(buffer, &buffer_length);

for (doc = 0; doc < search_engine.document_count(); doc++)
	doclist.puts(filenames[doc]);

doclist.close();

if (!silent)
	printf("%lld IDs written to %s\n", search_engine.document_count(), doclist_aspt);

return 0;
}
