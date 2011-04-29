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
#include <climits>

#include <icstring.hpp>

#include "word.h"
#include "uniseg_types.h"
#include "freq_file.h"

class Freq {
public:
	//typedef std::map<string_type, word_ptr_type, ignorcase_string_compare>
	typedef std::map<string_type, word_ptr_type>
													freq_type;

private:
	/// for more than one character (2, 3, 4, 5 ...)
	freq_type					freq_;
	/// the single character which is separated by the known boundaray
	/// like punctuation, sybomls and other character in different lanaguages
	array_type					freq_1_;
	array_array_type			freq_n_;

	std::vector<unsigned int>	sum_n_;
	std::vector<double>			avg_n_;
	int 						k_;

	bool 						loaded_;
	std::map<int, FreqFile *> 	freq_files_;

	long						current_document_id_;

	long						number_of_documents_;
	long						average_document_length_;

	std::vector<word_ptr_type>	doc_;
	UNISEG_encoding 			*enc_;

	bool						count_all_char_;

public:
	Freq();
	virtual ~Freq();

	freq_type& set() { return freq_; }

	word_ptr_type find(string_type word);
	bool has_word(string_type word);

	word_ptr_type add(string_array& ca, long lang = 0, unsigned int freq = -1, bool allnew = false);
	//word_ptr_type add(string_array& ca, unsigned int freq = -1, bool allnew = false);
	void add(word_ptr_type word_ptr, bool allnew = false);
	void add_to_array(word_ptr_type word_ptr);

	void sort(int k);
	void show(int k, int, bool details = false);
	void showcol(int k, int, bool details);
	void show();

	void show_oov();

	void alloc(int k = 1);  /// allocate the address

	int size() { return freq_.size(); }
	int array_size() { return k_; /*freq_n_.size();*/ }
	int array_k_size(int k) {
		assert(k < (int)freq_n_.size());
		return freq_n_[k].size();
	}

	array_type& array_k(int k) { return freq_n_[k]; }
	void array_to_array(array_type& wa, string_array& ca);

	std::map<int, FreqFile *>& freq_files() { return freq_files_; }

	std::vector<unsigned int>& sum_array() { return sum_n_; }
	std::vector<double>& avg_array() { return avg_n_; }
	unsigned int sum_k(int k) { if (sum_n_[k] == 0) cal_sum_k(k); return sum_n_[k]; }
	long sum();
	double avg_k(int k) { return avg_n_[k]; }

	void cal_sum();
	void cal_sum_k(int k);
	void cal_avg();
	void cal_sum_n_avg();

	void assign_freq(Freq& freq);
	void assign_freq_for_segmentation(Freq& freq, double base, bool joint_freq);
	void assign_freq_for_segmentation(word_ptr_type local_word, word_ptr_type global_word, bool joint_freq);
	void set_seged(Freq& freq, unsigned int freqc);
	void reduce_freq(Freq& freq, word_ptr_type tw_ptr);
	void add_freq(Freq& freq, int threshold = 1);
	void add_word_freq(word_ptr_type tw_ptr, unsigned int freq);

	void justify(unsigned int freq);
	void smooth(bool only_substr = false);
	void smooth(int k, bool only_substr);
	void smooth_passage();
	void extend(int k);

	void cal_word_p(double base);
	void show_p();
	void cal_word_a();
	void cal_word_ngmi_a(double base);

	void cal_word();

	void clear() { freq_.clear(); }

	void to_array(std::vector<word_ptr_type>& wa);
	void clone(Freq& freq);
	void merge(Freq& freq);
	void mergeto(Freq& freq);

	void pile_up(int max);

	void load(word_ptr_type word);
	void load(int index = -1);

	void load_freq(std::string path, int n = INT_MAX, bool force = false);

	void set_average_document_length(long length) { average_document_length_ =  length; }
	long get_average_document_length() { return average_document_length_; }

	void set_number_of_documents(long number) { number_of_documents_ = number; }
	long get_number_of_documents() { return number_of_documents_; }
	void increase_number_of_documents() { ++number_of_documents_; }

	void set_current_document_id(long id);

	void count_doc(std::string& doc, bool clean, bool smoothit);
	void count_doc(const char *doc, long len, bool clean, bool smoothit);

	void check_oov(word_ptr_type word, int threshold);

private:

	void remove_low(int k);
	static bool need_skip(word_ptr_type w_ptr);
};

#endif /*FREQ_H_*/
