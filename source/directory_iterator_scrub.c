/*
	DIRECTORY_ITERATOR_SCRUB.C
	--------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include "directory_iterator_scrub.h"
#include "unicode.h"

/*
	ANT_DIRECTORY_ITERATOR_SCRUB::ANT_DIRECTORY_ITERATOR_SCRUB()
	------------------------------------------------------------
*/
ANT_directory_iterator_scrub::ANT_directory_iterator_scrub(ANT_directory_iterator *source, long long scrubbing, long get_file) : ANT_directory_iterator("", get_file)
{
this->source = source;
this->scrubbing = scrubbing;
}

/*
	ANT_DIRECTORY_ITERATOR_SCRUB::~ANT_DIRECTORY_ITERATOR_SCRUB()
	-------------------------------------------------------------
*/
ANT_directory_iterator_scrub::~ANT_directory_iterator_scrub()
{
delete source;
}

/*
	ANT_DIRECTORY_ITERATOR_SCRUB::SCRUB()
	-------------------------------------
*/
void ANT_directory_iterator_scrub::scrub(unsigned char *data, long long size, long long scrubbing)
{
long long i = 0, byte, bytes, bytes_found;

for (; i < size; i++)
	if (scrubbing & NUL && *(data + i) == '\0')
		*(data + i) = ' ';
	if (scrubbing & NON_ASCII && *(data + i) & 0x80)
		*(data + i) = ' ';
	if (scrubbing & UTF8 && *(data + i) >= 0x80)
		{
		if ((*(data + i) & 0xC0) == 0x80)
			{
			/*
				A wild continuation byte appeared!
			*/
			*(data + i) = ' ';
			}
		else if (*(data + i) >= 0xD8 && *(data + i) < 0xE0)
			{
			/*
				Invalid high and low surrogate halves used by UTF-16 ... http://en.wikipedia.org/wiki/Utf8#Invalid_code_points
			*/
			*(data + i) = ' ';
			}
		else if ((*(data + i) == 0xF4 && utf8_to_wide(data + i) > 0x10FFFF) || *(data + i) > 0xF4)
			{
			/*
				These sequences are invalid 4-byte sequences, chomp the first byte, and let
				the wild continuation byte chomper chomp the rest of it
			*/
			*(data + i) = ' ';
			}
		else if (*(data + i) == 0xC0 || *(data + i) == 0xC1)
			{
			/*
				Overlong encoding of an ASCII character, so replace with correct encoding
				The wild continuation byte chomper will chomp the second byte
			*/
			*(data + i) = (char)utf8_to_wide(data + i);
			/*
				Of course we could have just deoverlongified a NUL, so check that
			*/
			if (*(data + i) == '\0')
				*(data + i) = ' ';
			}
		else if (i + 1 < size && (
			(*(data + i) == 0xE0 && (*(data + i + 1) & 0xE0) == 0x80) ||
			(*(data + i) == 0xF0 && (*(data + i + 1) & 0xF0) == 0x80) ||
			(*(data + i) == 0xF8 && (*(data + i + 1) & 0xF8) == 0x80) ||
			(*(data + i) == 0xFC && (*(data + i + 1) & 0xFC) == 0x80)
			))
			{
			/*
				An overlong encoding of a non-ASCII character, decode and re-encode to the 
				same space, wild continuation bytes will be left at the end and chomped later
				
				If the overlong encoded character is valid, decode and re-encode it
				otherwise, remove it
			*/
			if (isutf8(data + i))
				i += wide_to_utf8(data + i, size, utf8_to_wide(data + i)) - 1;
			else
				*(data + i) = ' ';
			}
		else
			{
			/*
				Check that we have all the required continuation bytes for this sequence
			*/
			bytes = utf8_bytes(data + i);
			
			/*
				Make sure that each byte following that should have the high bit set do have the high bit set
			*/
			for (bytes_found = byte = 1; byte < bytes && i + byte < size; byte++, bytes_found++)
				if ((*(data + i + byte) & 0xC0) != 0x80)
					break;
			
			/*
				If we didn't find all the continuation bytes, then replace all those bytes with a space
			*/
			if (bytes_found != bytes)
				for (byte = 0; byte < bytes_found; byte++)
					*(data + i + byte) = ' ';
			
			/*
				Skip over all the continuation bytes we found
			*/
			i += bytes_found - 1;
			}
		}
}

/*
	ANT_DIRECTORY_ITERATOR_SCRUB::FIRST()
	-------------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_scrub::first(ANT_directory_iterator_object *object)
{
ANT_directory_iterator_object *t = source->first(object);

if (t != NULL)
	scrub((unsigned char *)t->file, t->length, scrubbing);

return t;
}

/*
	ANT_DIRECTORY_ITERATOR_SCRUB::NEXT()
	------------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_scrub::next(ANT_directory_iterator_object *object)
{
ANT_directory_iterator_object *t = source->next(object);

if (t != NULL)
	scrub((unsigned char *)t->file, t->length, scrubbing);

return t;
}
