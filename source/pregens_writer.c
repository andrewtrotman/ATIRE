/*
	PREGENS_WRITER.C
	----------------
*/
#include "pregens_writer.h"
#include "pregen_writer_normal.h"
#include "pregen_writer_exact_strings.h"
#include "pregen_writer_exact_integers.h"
/*
	ANT_PREGENS_WRITER::ANT_PREGENS_WRITER()
	----------------------------------------
*/
ANT_pregens_writer::ANT_pregens_writer()
{
field_count = 0;
}

/*
	ANT_PREGENS_WRITER::~ANT_PREGENS_WRITER()
	-----------------------------------------
*/
ANT_pregens_writer::~ANT_pregens_writer()
{
for (int i = 0; i < field_count; i++)
	delete fields[i];
}

/*
	ANT_PREGENS_WRITER::ADD_FIELD()
	-------------------------------
*/
int ANT_pregens_writer::add_field(const char *filename, const char *field_name, ANT_pregen_field_type type)
{
ANT_pregen_writer * field;

switch (type)
	{
	case STREXACT:
		field = new ANT_pregen_writer_exact_strings(field_name, 0);
		break;
	case STREXACT_RESTRICTED:
		field = new ANT_pregen_writer_exact_strings(field_name, 1);
		break;
	case INTEGEREXACT:
		field = new ANT_pregen_writer_exact_integers(field_name);
		break;
	default:
		field = new ANT_pregen_writer_normal(type, field_name);
	}

if (!field->open_write(filename))
	{
	delete field;
	return 0;
	}

fields[field_count] = field;
field_count++;

return 1;
}


/*
	ANT_PREGENS_WRITER::PROCESS_DOCUMENT()
	--------------------------------------
*/
void ANT_pregens_writer::process_document(long long doc_index, ANT_string_pair doc_name)
{
//TODO version which doesn't cause copying...
char *dup = doc_name.str();

process_document(doc_index, dup);

delete [] dup;
}

/*
	ANT_PREGENS_WRITER::STRIP_GARBAGE()
	-----------------------------------
*/
ANT_string_pair ANT_pregens_writer::strip_garbage(const ANT_string_pair & s)
{
ANT_string_pair result = s;

/*
	Encoders will strip spaces themselves, but do it here too so we can
	get down to prefixes we actually want to strip.
*/
while (result.string_length > 0 && *result.start == ' ')
	{
	result.start++;
	result.string_length--;
	}

/*
	Strip off leading "Re:"
*/
if (result.string_length >=2 && (result[0] == 'R' || result[0] == 'r')
		&& (result[1] == 'E' || result[1] == 'e'))
		{
		if (result.string_length >= 3 && result[2] == ':')
			{
			result.string_length -= 3;
			result.start += 3;
			}
		else
			{
			result.string_length -= 2;
			result.start += 2;
			}
		}

return result;
}

/*
	ANT_PREGENS_WRITER::PROCESS_DOCUMENT()
	--------------------------------------
*/
void ANT_pregens_writer::process_document(long long doc_index, char *doc_name)
{
char * pos = doc_name;
enum { IDLE, INSIDE_TAG, INSIDE_OPEN_TAG, INSIDE_CLOSE_TAG} state = IDLE;

int tag_depth = 0;
ANT_string_pair tag_name(NULL, 0), tag_body(NULL, 0), close_tag_name;

/*
	Parse document name and find matching top-level XML fields to use
*/
while (*pos)
	{
	switch (state)
		{
		case IDLE:
			if (*pos == '<')
				{
				tag_body.string_length = pos - tag_body.start;
				state = INSIDE_TAG;
				}
			break;
		case INSIDE_TAG:
			if (*pos == '/')
				{
				state = INSIDE_CLOSE_TAG;
				close_tag_name.start = pos + 1;
				break;
				}

			tag_name.start = pos;
			state = INSIDE_OPEN_TAG;
			//Fall-through

		case INSIDE_OPEN_TAG:
			if (*pos == '>')
				{
				tag_name.string_length = pos - tag_name.start;
				tag_body.start = pos + 1;
				tag_depth++;
				state = IDLE;
				}
			break;

		case INSIDE_CLOSE_TAG:
			if (*pos == '>')
				{
				tag_depth--;

				if (tag_depth == 0)
					{
					for (int i = 0; i < field_count; i++)
						if (tag_name.true_strcmp(fields[i]->field_name) == 0)
							fields[i]->add_field(doc_index, strip_garbage(tag_body));
					state = IDLE;
					}
				}
			break;
		}
	++pos;
	}
}

/*
	ANT_PREGENS_WRITER::CLOSE()
	---------------------------
*/
void ANT_pregens_writer::close()
{
for (int i = 0; i < field_count; i++)
	fields[i]->close_write();
}

