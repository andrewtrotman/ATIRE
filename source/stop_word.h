/*
	STOP_WORD.H
	-----------
*/
#ifndef STOP_WORD_H_
#define STOP_WORD_H_

class ANT_stop_word
{
public:
	enum {NCBI, PUURULA};

private:
	static char **ANT_stop_word_list;
	static long ANT_stop_word_list_len;

	static char *ANT_NCBI_stop_word_list[];
	static long ANT_NCBI_stop_word_list_len;
	static char *ANT_puurula_stop_word_list[];
	static long ANT_puurula_stop_word_list_len;

	char **extra_stop;
	long extra_stop_length;
	long type;

public:
	ANT_stop_word(long which_stop_word_list = NCBI);
	virtual ~ANT_stop_word();

	virtual long addstop(const char *term);
	virtual long addstop(const char **term);

	virtual long isstop(const char *term);
	virtual long isstop(const char *term, long len);

	long get_type(void) { return type; }
} ;


#endif  /* STOP_WORD_H_ */
