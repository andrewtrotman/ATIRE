/*
	 WORD.H
	 ------
 */

#ifndef __WORD_H__
#define __WORD_H__

#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <map>
#include <cassert>

#include "icstring.hpp"
#include "encoding.h"
#include "address.h"

class Word
{
public:
	enum { INVALID = -1 };
	enum Side { LEFT = 0, RIGHT = 1, UNKNOWN = 2 };

	typedef std::pair<int, unsigned int> 			IndexFreqPair;

public:
	typedef Word									word_type;
	typedef word_type								*word_ptr_type;
	typedef std::vector<word_ptr_type>				array_type;

private:
	string_type 									chars_;

	unsigned int 									freq_;
	int 											address_;
	int 											size_; /// number of characters

	array_type 										arr_; /// the Word *array mapping to each parent character(s)
	array_type										children_; /// the Word *array linked to children character(s)
	std::map<std::string, Word*> 					rcchar_;  /// the right single characters of all its children
	Word 											*lparent_;
	Word 											*rparent_;
	Word 											*lchar_;
	Word 											*rchar_;

	Side 											side_; // the lefe or right indicator of parent with the highest frequency

	unsigned int 									lcounter_;
	unsigned int 									rcounter_;

	bool 											is_word_;
	bool 											seged_;

	double											p_;
	//double											a_;
	double											left_a_;
	double											right_a_;
	double											base_;

	Word*											pre_;
	Word*											next_;

	UNISEG_encoding::language						lang_;
	Address::uint_array								disk_address_;
	bool											loaded_;

private:
	void init();

public:
	Word(string_type chars, int size);
	Word(string_type chars, int freq, int size);
	~Word() {}

	const Word* pre() const { return pre_; }
	const Word* next() const { return next_; }
	void pre(Word* word) { pre_ = word; }
	void next(Word* word) { next_ = word; }

	void lang(UNISEG_encoding::language lang) { lang_ = lang; }
	UNISEG_encoding::language lang() { return lang_; }

	Word& operator ++ ();
	Word operator ++ (int);
	bool operator < (Word *w2);
	bool operator() (int);

	const unsigned int freq() const { return freq_; }
	void freq(unsigned int freq) { freq_ = freq; }

	const int address() const { return address_; }
	void address(int address) { address_ = address; }

	Address::uint_array& disk_address() { return disk_address_; }
	//void address(int address) { disk_address_ = disk_address; }

	/*due to the fact that the unicode string use more bytes than just one byte, we cann't do this:
	 * chars_.length(); */
	const int size() const { return size_; }
	void size(int size) { size_ = size; }

	unsigned int lcounter() { return lcounter_; }
	unsigned int rcounter() { return rcounter_; }
	int counter(Side side);

	void lcounter(unsigned int count) { lcounter_ = count; }
	void rcounter(unsigned int count) { rcounter_ = count; }
	void counter(Side side, unsigned int count);

	const string_type& chars() const { return chars_; }
	string_type& chars() { return chars_; }

	string_type subchars(int idx, int len);
	void subchars(string_type& substr, int idx, int len);

	void subarray(array_type& ca, int idx, int len);
	Word *subword(int idx, int len);

	void increase() { freq_++; }
	void increase(unsigned int freq) { freq_ += freq; }
	void link(Word *w) { children_.push_back(w); }
	void add(Word *w) { arr_.push_back(w); }
	const std::vector<Word*>& array() const { return arr_; }
	void array(std::vector<Word*> arr);

	//Word *word(int idx);
	unsigned int highest_freq();
	static bool cmp_freq(Word *w1, Word *w2);
	static bool cmp_just_freq(Word *w1, Word *w2);

	const Word *lparent() const { return lparent_; }
	Word *lparent() { return lparent_; }
	void lparent(Word *lparent) {
		lparent_ = lparent;
		if (lparent_ != NULL && lparent_ != this) {
			lparent_->link(this);
		}
	}
	const Word *rparent() const { return rparent_; }
	Word *rparent() { return rparent_; }
	void rparent(Word *rparent) {
		rparent_ = rparent;
		if (rparent_ != NULL && rparent != this)
			rparent_->link(this);
	}
	const Word *lchar() const { return lchar_; }
	void lchar(Word *lchar) {
		assert(lchar->size() == 1);
		lchar_ = lchar;
	}

	const Word *rchar() const { return rchar_; }
	void rchar(Word *rchar) {
		//assert(this->lparent_ != NULL);
		rchar_ = rchar;
		if (this->lparent_ != NULL)
			lparent_->add_rcchar(rchar_, this);
	}

	void add_rcchar(Word *rchar, Word *child) {
		assert(rchar->size() == 1);
		rcchar_.insert(make_pair(rchar->chars(), child));
	}

	/**
	 * find a direct right child
	 * @param a_char the most right char of the word
	 */
	Word *find_rchild(string_type a_char);

	void children_do(void (*function_ptr)(Word*));

	std::pair<Word*, Word*> family();
	const Word *parent() const;
	const Word *parent2() const; /// return the parent with less frequency
	bool has_parent();

	unsigned int children_freq();
	const Side side() const;

	static string_type array_to_string(array_type& wa);
	static string_type array_to_string(array_type& wa, int idx, int len);
	static string_array array_to_array(const array_type& wa);

	void to_string_array(string_array& ca);

	bool is_word() { return is_word_; }
	void is_word(bool b);
	bool seged() { return seged_; }
	void seged(bool b) { seged_ = b; }
	void seged_if(unsigned int freq);

	void justify(unsigned int min = 2);
	void reduce_freq(unsigned int freq, unsigned int min = 0);

	void cal_p(double base);
	void cal_a();

	double p() { return p_; }
	double a() { return left_a_ + right_a_; }
	double right_a() { return right_a_; }
	void right_a(double a) { right_a_ = a; }
	double left_a() { return left_a_; }
	void left_a(double a) { left_a_ = a; }

	void base(double base) { base_ = base; }
	double base() { return base_; }

private:
	double cal_a(int start);
};

inline const Word::Side Word::side() const {
	return side_;
}

typedef Word::word_type								word_type;
typedef Word::word_ptr_type							word_ptr_type;
typedef const word_ptr_type							const_word_ptr;
typedef Word::array_type							array_type;

typedef std::map<string_type, word_ptr_type>		freq_type;
typedef std::map<string_type, word_ptr_type>		word_map;

typedef std::vector<array_type >					array_array_type;

#endif /*__WORD_H__*/
