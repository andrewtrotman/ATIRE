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

public:
	ANT_stop_word() {};
	virtual ~ANT_stop_word() {};

	virtual long isstop(const char *term);
} ;


#endif __STOP_WORD_H__
