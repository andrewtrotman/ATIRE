/*
	INDEXER_PARAM_BLOCK_PREGEN.C
	----------------------------
*/
#include <stdio.h>
#include <stdlib.h>

#include "indexer_param_block_pregen.h"

/*
	ANT_INDEXER_PARAM_BLOCK_PREGEN::ANT_INDEXER_PARAM_BLOCK_PREGEN()
	----------------------------------------------------------------
*/
ANT_indexer_param_block_pregen::ANT_indexer_param_block_pregen()
{
num_pregen_fields = 0;
}

/*
	ANT_INDEXER_PARAM_BLOCK_PREGEN::~ANT_INDEXER_PARAM_BLOCK_PREGEN()
	-----------------------------------------------------------------
*/
ANT_indexer_param_block_pregen::~ANT_indexer_param_block_pregen()
{
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
else if (strcmp(field_type, "bintrunc") == 0)
	pregens[num_pregen_fields].type = BINTRUNC;
else if (strcmp(field_type, "5bit") == 0)
	pregens[num_pregen_fields].type = ASCII_5BIT;
else if (strcmp(field_type, "base36") == 0)
	pregens[num_pregen_fields].type = BASE36;
else if (strcmp(field_type, "base37") == 0)
	pregens[num_pregen_fields].type = BASE37;
else if (strcmp(field_type, "base40") == 0)
	pregens[num_pregen_fields].type = BASE40;
else if (strcmp(field_type, "base37arith") == 0)
	pregens[num_pregen_fields].type = BASE37_ARITHMETIC;
else if (strcmp(field_type, "asciiprintables") == 0)
	pregens[num_pregen_fields].type = ASCII_PRINTABLES;
else if (strcmp(field_type, "asciiprintablesarith") == 0)
	pregens[num_pregen_fields].type = ASCII_PRINTABLES_ARITHMETIC;
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
void ANT_indexer_param_block_pregen::help(void)
{
puts("-pregen <field> <type>  Create a pregenerated ranking from docid field of type:");
puts("   integer      Unsigned integer");
puts("   intexact     Rank by ordering on complete integers");
puts("   strexact     Rank by ordering on complete strings (cannot rank results from merged indexes)");
puts("   bintrunc     Rank by a simple prefix of the given string (binary safe)");
puts("   strtrunc     Rank by a prefix of a lowercase, accentless version of the string");
puts("   5bit         Rank by 5-bit conversion of alphanumerics with limited numeric sort precision");
puts("   base36       Rank by base-36 conversion of alphanumerics");
puts("   base37       Rank by base-37 conversion of alphanumerics and spaces");
puts("   base37arith  Rank by arithmetic encoding of alphanumerics and spaces with English model");
puts("   base40       Rank by base-40 conversion of alphanumerics and spaces, with basic sorting for punctuation");
puts("   asciiprintable Rank by printable ASCII");
puts("   asciiprintablesarith Rank by printable ASCII with English model");
puts("");
}

