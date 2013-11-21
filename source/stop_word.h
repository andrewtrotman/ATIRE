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


#endif  /* STOP_WORD_H_ */
