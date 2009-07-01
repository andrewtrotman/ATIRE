/*
 * range.h
 *
 *  Created on: Jan 28, 2009
 *      Author: monfee
 */

#ifndef RANGE_H_
#define RANGE_H_

class Range {
public:
	Range() : left_(0), right_(0){
		ro_ = false;
		lo_ = false;
		conf_ = false;
	}
	Range(unsigned int left, unsigned int right) : left_(left), right_(right){

	}
	~Range() {	}

	const unsigned int left() const { return left_; }
	const unsigned int right() const { return right_; }

	void left (unsigned int left) { left_ = left; }
	void right (unsigned int right) { right_ = right; }

	bool ro() { return ro_; }
	void ro(bool b) { ro_ = b; }

	bool lo() { return lo_; }
	void lo(bool b) { lo_ = b; }

	void conf(bool b) { conf_ = b; }

private:
	unsigned int 	left_;
	unsigned int 	right_;
	bool			ro_; // right overlapped
	bool			lo_; // right overlapped
	bool			conf_; // confirmed range, means it is a OK, acceptable word
};

struct range_compare : public std::binary_function<Range*, Range*, bool> {

    bool operator() (const Range* x, const Range* y) const
        { return x->left() < y->left(); }

};

#endif /* RANGE_H_ */
