/*
	PREGEN_WRITER.C
	---------------
*/
#include "pregen_writer.h"
#include "str.h"

/*
	ANT_PREGEN_WRITER::ANT_PREGEN_WRITER()
	--------------------------------------
*/
ANT_pregen_writer::ANT_pregen_writer(ANT_pregen_field_type type, const char *name)
{
field_name = strnew(name);
doc_count = 0;

this->type = type;
}
