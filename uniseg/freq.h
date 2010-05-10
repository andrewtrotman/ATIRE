/*
 * freq.h
 *
 *  Created on: Oct 26, 2008
 *      Author: monfee
 */

#ifndef FREQ_H_
#define FREQ_H_

#include <map>
#include <string>
#include <cassert>
#include <icstring.hpp>

#include "word.h"
#include "uniseg_types.h"

class Freq {
public:
	//typedef std::map<string_type, word_ptr_type, ignorcase_string_compare>
	typedef std::map<string_type, word_ptr_type>
													freq_type;

private:
	/// for more than one character (2, 3, 4, 5 ...)
	freq_type	freq_;

	/// the single character which is separated by the known boundaray
	/// like punctuation, sybomls and other character in different lanaguages
	array_type	freq_1_;
	array_array_type	freq_n_;
	std::vector<unsigned int>	sum_n_;
	std::vector<double>	avg_n_;
	int k_;

public:
	Freq();
	virtual ~Freq();

	freq_type& set() { return freq_; }

	word_ptr_type find(string_type word);
	bool has_word(string_type word);

	word_ptr_type add(string_array& ca, long lang, unsigned int freq = -1, bool allnew = false);
	void add(word_ptr_type word_ptr, bool allnew = false);

	void sort(int k);
	void show(int k, int min = 1);
	void showcol(int k, int min = 1);
	void show();
	void alloc(int k = 1);  /// allocate the address

	int size() { return freq_.size(); }
	int array_size() { return k_; /*freq_n_.size();*/ }
	int array_k_size(int k) {
		assert(k < (int)freq_n_.size());
		return freq_n_[k].size();
	}

	array_type& array_k(int k) { return freq_n_[k]; }
	void array_to_array(array_type& wa, string_array& ca);

	std::vector<unsigned int>& sum_array() { return sum_n_; }
	std::vector<double>& avg_array() { return avg_n_; }
	unsigned int sum_k(int k) { return sum_n_[k]; }
	double avg_k(int k) { return avg_n_[k]; }

	void cal_sum();
	void cal_avg();
	void cal_sum_n_avg();

	void assign_freq(Freq& freq);
	void set_seged(Freq& freq, unsigned int freqc);
	void reduce_freq(Freq& freq, word_ptr_type tw_ptr);
	void add_freq(Freq& freq, int threshold = 1);

	void justify(unsigned int min = 2);

	void cal_word_p(double base);
	void show_p();
	void cal_word_a();

	void clear() { freq_.clear(); }

	void to_array(std::vector<word_ptr_type>& wa);
	void clone(Freq& freq);
	void merge(Freq& freq);
	void mergeto(Freq& freq);

	void pile_up(int max);
private:

	void remove_low(int k);
	static bool need_skip(word_ptr_type w_ptr);
};

#endif /*FREQ_H_*/
