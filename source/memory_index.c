/*
	MEMORY_INDEX.C
	--------------
*/
#include <string.h>

class ANT_memory_index
{
private:
	ANT_memory_index_hash_node *hash_table[27*27*27*27];

private:
	long hash(ANT_string_pair *string);

public:
	ANT_memory_index();
	~ANT_memory_index();

	void add_term(ANT_string_pair *string, long docno);
} ;

/*
	ANT_MEMORY_INDEX::ANT_MEMORY_INDEX()
	------------------------------------
*/
ANT_memory_index::ANT_memory_index()
{
memset(hash_table, 0, sizeof(hash_table));
}

/*
	ANT_MEMORY_INDEX::~ANT_MEMORY_INDEX()
	-------------------------------------
*/
ANT_memory_index::~ANT_memory_index()
{
}

/*
	ANT_MEMORY_INDEX::HASH()
	------------------------
*/
long ANT_memory_index::hash(ANT_string_pair *string)
{
long ans, len;

ans = string[0];
if ((len = string->length()) > 1)
	ans += string[1] * 27;
if (len > 2)
	ans += string[2] * 27 * 27;
if (len > 3)
	ans += string[3] * 27 * 27 * 27;

return ans;
}

/*
	ANT_MEMORY_INDEX::ADD_TERM()
	----------------------------
*/
void ANT_memory_index::add_term(ANT_string_pair *string, long docno)
{
long hash_value;

hash_value = hash(string);
if (hash_table[hash_value] == NULL)
	hast_table[hash_value] = new ANT_memory_index_hash_node(string);
/*
	now navigate the binary tree
	now add to the end of the postings list
*/
}

