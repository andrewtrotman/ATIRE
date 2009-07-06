/*
 * seg.h
 *
 *  Created on: Nov 29, 2008
 *      Author: monfee
 */

#ifndef SEG_H_
#define SEG_H_

#include <string>
#include "uniseg_types.h"
#include "freq.h"
#include "clist.h"

class Seger {
private:
	CList		clist_;
	Freq		freq_;
	Freq&		allfreq_;
	word_ptr_type tw_ptr_;
	const string_type& stream_;  // for the incoming document which will be segmented

public:
	Seger(Freq&	allfreq, word_ptr_type tw_ptr);
	Seger(Freq&	allfreq, const string_type& stream);
	~Seger();

	void start();
	void output();

	void build();
	void show_all();
	void seg();
	void add_to_list(array_type& cwlist);
	void mark_the_seged();

	/**
	 * simple segmentation algorightm #1
	 */
	void s1(Freq::array_type);

private:
	void make(CList& clist, string_type& str);
	void assign_freq();
	void justify(unsigned int min = 2);
	void do_some_calculations();
};

#endif /* SEG_H_ */
