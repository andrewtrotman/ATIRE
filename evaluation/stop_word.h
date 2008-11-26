/*
	STOP_WORD.H
	-----------
*/
#ifndef __STOP_WORD_H__
#define __STOP_WORD_H__

class ANT_stop_word
{
private:
	static char *ANT_stop_word_list[];
	static long ANT_stop_word_list_len;
	char **extra_stop;
	long extra_stop_length;
	char *buffer;
	long len;

public:
	ANT_stop_word();
	virtual ~ANT_stop_word();

	virtual long addstop(const char *term);
	virtual long addstop(const char **term);

	virtual long isstop(const char *term);
	virtual long isstop(const char *term, long len);
} ;


#endif __STOP_WORD_H__
