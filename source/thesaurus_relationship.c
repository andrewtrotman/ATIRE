/*
	THESAURUS_RELATIONSHIP.C
	------------------------
*/
#include <sstream>
#include "str.h"
#include "thesaurus_relationship.h"

/*
	GET_RELATIONSHIP_NAME()
	-----------------------
*/
char *ANT_thesaurus_relationship::get_relationship_name(long relationship)
{
std::ostringstream out;

if (relationship == 0)
	out << "NONE";

if (relationship & HYPONYM)
	out << "HYPONYM ";

if (relationship & ANTONYM)
	out << "ANTONYM ";

if (relationship & HOLONYM)
	out << "HOLONYM ";

if (relationship & MERONYM)
	out << "MERONYM ";

if (relationship & HYPERNYM)
	out << "HYPERNYM ";

if (relationship & SYNONYM)
	out << "SYNONYM ";

return strnew(out.str().c_str());
}
