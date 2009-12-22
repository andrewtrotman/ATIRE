/*
 * stop_word.h
 *
 *  Created on: Dec 22, 2009
 *      Author: monfee
 */

#ifndef STOP_WORD_H_
#define STOP_WORD_H_

class StopWord {
public:
	static char *chinese_stop_word_list[];
	static long chinese_stop_word_list_len;

public:
	StopWord();
	virtual ~StopWord();

	static bool is_chinese_stop_word(const char *term);
};

#endif /* STOP_WORD_H_ */
