/*
 * seg2.h
 *
 *  Created on: May 20, 2010
 *      Author: monfee
 */

#ifndef SEG2_H_
#define SEG2_H_

#include "seg.h"
#include "uniseg_string.h"

class Seger2 : public Seger
{
private:
	std::vector<double>		boundary_score_;

public:
	Seger2();
	Seger2(const string_type stream);
	Seger2(const char* stream, size_t length);
	virtual ~Seger2();

	std::vector<double>& boundary_score() { return boundary_score_; }

	void find_boundary(std::vector<double>::iterator begin, std::vector<double>::iterator end, word_ptr_type current_word, std::string& to_become);

protected:
	virtual void seg();
	virtual void build();
	void add_to_list(array_type& cwlist);
	void do_some_calculations();
};

#endif /* SEG2_H_ */
