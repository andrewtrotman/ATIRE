/*
	COMPRESS_TEXT.H
	---------------
*/
#ifndef COMPRESS_TEXT_H_
#define COMPRESS_TEXT_H_

/*
	class ANT_COMPRESS_TEXT
	-----------------------
*/
class ANT_compress_text
{
public:
	ANT_compress_text() {}
	virtual ~ANT_compress_text() {}

	virtual char *compress(char *destination, unsigned long *destination_length, char *source, unsigned long source_length) = 0;
	virtual char *decompress(char *destination, unsigned long *destination_length, char *source, unsigned long source_length) = 0;
	virtual unsigned long space_needed_to_compress(unsigned long source_length) { return source_length; }
} ;

#endif /* COMPRESS_TEXT_H_ */
