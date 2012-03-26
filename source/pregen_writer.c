/*
	PREGEN_WRITER.C
	---------------
*/
#include "pregen_t.h"
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

/*
	ANT_PREGEN_WRITER::OPEN_WRITE()
	-------------------------------
*/
int ANT_pregen_writer::open_write(const char *filename)
{
if (file.open(filename, "wbx") == 0)
	return 0;

file.write((unsigned char *)file_header, sizeof(file_header));

file.write((unsigned char *)field_name, strlen(field_name) * sizeof(*field_name));

return 1;
}

/*
	ANT_PREGEN_WRITER::CLOSE_WRITE()
	--------------------------------
*/
void ANT_pregen_writer::close_write()
{
uint32_t four_byte;

four_byte = PREGEN_FILE_VERSION;
file.write((unsigned char *)&four_byte, sizeof(four_byte));
four_byte = doc_count;
file.write((unsigned char *)&four_byte, sizeof(four_byte));
four_byte = sizeof(ANT_pregen_t);
file.write((unsigned char *)&four_byte, sizeof(four_byte));
four_byte = type;
file.write((unsigned char *)&four_byte, sizeof(four_byte));
four_byte = (uint32_t)strlen(field_name);
file.write((unsigned char *)&four_byte, sizeof(four_byte));

file.close();
}
