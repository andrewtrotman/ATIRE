/*
	INDEXER_PARAM_BLOCK_PREGEN.C
	--------------------------
*/
#include <stdio.h>
#include <stdlib.h>

#include "indexer_param_block_pregen.h"


/*
	ANT_INDEXER_PARAM_BLOCK_PREGEN::~ANT_INDEXER_PARAM_BLOCK_PREGEN()
	-----------------------------------------------------------------
*/
ANT_indexer_param_block_pregen::~ANT_indexer_param_block_pregen()
{

}

/*
	ANT_INDEXER_PARAM_BLOCK_PREGEN::ANT_INDEXER_PARAM_BLOCK_PREGEN()
	----------------------------------------------------------------
*/
ANT_indexer_param_block_pregen::ANT_indexer_param_block_pregen()
{
num_pregen_fields = 0;
}

/*
	ANT_INDEXER_PARAM_BLOCK_PREGEN::ADD_PREGEN_FIELD()
	--------------------------------------------------
*/
int ANT_indexer_param_block_pregen::add_pregen_field(char *field_name, char *field_type)
{
	if (strcmp(field_type, "integer") == 0)
		pregens[num_pregen_fields].type = INTEGER;
	else if (strcmp(field_type, "intexact") == 0)
		pregens[num_pregen_fields].type = INTEGEREXACT;
	else if (strcmp(field_type, "strexact") == 0)
		pregens[num_pregen_fields].type = STREXACT;
	else if (strcmp(field_type, "strtrunc") == 0)
		pregens[num_pregen_fields].type = STRTRUNC;
	else if (strcmp(field_type, "asciidigest") == 0)
		pregens[num_pregen_fields].type = ASCIIDIGEST;
	else if (strcmp(field_type, "base36") == 0)
		pregens[num_pregen_fields].type = BASE36;
	else
		return 0;

	pregens[num_pregen_fields].field_name = field_name;

	num_pregen_fields++;

	return 1;
}

/*
	ANT_INDEXER_PARAM_BLOCK_PREGEN::HELP()
	--------------------------------------
*/
void ANT_indexer_param_block_pregen::help()
{
puts("-pregen <field> <type>  Create a pregenerated ranking from docid field of type:");
puts("   integer      Unsigned integer");
puts("   intexact     Rank by ordering on complete integers");
puts("   strexact     Rank by ordering on complete strings (cannot rank results from merged indexes)");
puts("   strtrunc     Rank by a simple prefix of the given string (binary safe)");
puts("   asciidigest  Rank by compact ASCII conversion of the prefix of the given string");
puts("   base36       Rank by base-36 conversion of alphanumerics");
puts("   recentdate   Rank by recently-biased UNIX timestamps");
puts("");
}

