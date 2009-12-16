/*
 * address.h
 *
 *  Created on: Oct 27, 2008
 *      Author: monfee
 */

#ifndef ADDRESS_H_
#define ADDRESS_H_

#include <string>
#include <vector>
#include <algorithm>

#include "uniseg_string.h"

class Address {
public:
	typedef unsigned int						uint;
	typedef std::vector<uint>					uint_array;
	enum { INVALID_BOUND = 0xffffffff };

private:
	string_array	ca_;
	uint_array		bound_;

public:
	Address(string_array ca, uint_array bound) :
		ca_(ca), bound_(bound) {};
	~Address() {};

	uint_array& bound() { return bound_; }
	void bound(uint_array& bound) { std::copy(bound.begin(), bound.end(), bound_.begin()); }

	void add(string_type a_char) { ca_.push_back(a_char); }
	string_array& ca() { return ca_; }
};

typedef std::vector<Address> address_array;

#endif /* ADDRESS_H_ */
