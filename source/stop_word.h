/*
	STOP_WORD.H
	-----------
*/
#ifndef STOP_WORD_H_
#define STOP_WORD_H_

class ANT_stop_word
{
private:
	static char *ANT_stop_word_list[];
	static long ANT_stop_word_list_len;

public:
	ANT_stop_word() {};
	virtual ~ANT_stop_word() {};

	static long isstop(const char *term);
} ;


#endif  /* STOP_WORD_H_ */
