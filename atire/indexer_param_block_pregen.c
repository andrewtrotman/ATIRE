/*
	INDEXER_PARAM_BLOCK_PREGEN.C
	----------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
else if (strcmp(field_type, "strexactrestricted") == 0)
	pregens[num_pregen_fields].type = STREXACT_RESTRICTED;
else if (strcmp(field_type, "strtrunc") == 0)
	pregens[num_pregen_fields].type = STRTRUNC;
else if (strcmp(field_type, "bintrunc") == 0)
	pregens[num_pregen_fields].type = BINTRUNC;
else if (strcmp(field_type, "base32") == 0)
	pregens[num_pregen_fields].type = BASE32;
else if (strcmp(field_type, "base32arith") == 0)
	pregens[num_pregen_fields].type = BASE32_ARITHMETIC;
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
else if (strcmp(field_type, "asciiprintablesarithbigram") == 0)
	pregens[num_pregen_fields].type = ASCII_PRINTABLES_ARITHMETIC_BIGRAM;
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
puts("   intexact     Rank by sorting complete integers");
puts("   strexact     Rank by sorting complete strings (cannot rank results from merged indexes)");
puts("   bintrunc     Prefix of the given string (binary safe)");
puts("   strtrunc     Prefix of a lowercase, accentless version of the string");
puts("   base32       Alphanumerics and spaces, limited precision for digits");
puts("   base32arith  Alphanumerics and spaces, limited precision for digits, English model");
puts("   base36       Alphanumerics");
puts("   base37       Alphanumerics and spaces");
puts("   base37arith  Alphanumerics and spaces with English probability model");
puts("   base40       Alphanumerics and spaces, with basic sorting for punctuation");
puts("   asciiprintable All printable ASCII characters");
puts("   asciiprintablesarith All printable ASCII characters with unigram English probability model");
puts("   asciiprintablesarithbigram All printable ASCII characters with bigram English probability model");
puts("");
}

