/*
	THESAURUS_ROOTNODE.H
	--------------------
*/
#include <string.h>

/*
	class ANT_THESAURUS_ROOTNODE
	----------------------------
*/
class ANT_thesaurus_rootnode
{
public:
	char *name;
	long long start;
	long long length;

public:
	static int string_compare(const void *a, const void *b) { return strcmp((char *)a, ((ANT_thesaurus_rootnode *)b)->name); }
} ;