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
	Seger2(const string_type stream);
	Seger2(const char* stream, size_t length);
	virtual ~Seger2();

protected:
	virtual void seg();
	virtual void build();
	void add_to_list(array_type& cwlist);
};

#endif /* SEG2_H_ */
