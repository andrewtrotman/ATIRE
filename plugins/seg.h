/*
 * SEG.H
 * -----
 *
 *  Created on: Nov 29, 2008
 *      Author: monfee
 */

#ifndef __SEG_H__
#define __SEG_H__

#include <string>
#include "uniseg_types.h"
#include "freq.h"
#include "clist.h"

class Seger {
private:
	CList				clist_;
	Freq				freq_;    // the frequency records for the input text
	Freq*				allfreq_;
	word_ptr_type 		tw_ptr_;
	string_type 		stream_;  // for the incoming document which will be segmented

public:
	Seger(word_ptr_type tw_ptr);
	Seger(const string_type stream);
	Seger(const char* stream, size_t length);
	~Seger();

	void start();
	void output();

	void build();
	void show_all();
	void seg();
	void add_to_list(array_type& cwlist);
	void mark_the_seged();

	string_type& stream() { return stream_; }

private:
	void init();
	void make(CList& clist, string_type& str);
	void assign_freq();
	void justify(unsigned int min = 2);
	void do_some_calculations();
};

#endif /* __SEG_H__ */