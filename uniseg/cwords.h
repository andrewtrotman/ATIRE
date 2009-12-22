#ifndef CWORDS_H_
#define CWORDS_H_

#include <list>
#include "uniseg_types.h"
#include "freq.h"

class CWords {
private:
	bool end_;
	int len_;
	int pos_;
	std::list<word_ptr_type>  cwords_;

	double score_;
	static double highest_s_;
	static double lowest_s_;

public:
	CWords(int, word_ptr_type);
	CWords(const CWords& cwords);

	~CWords();

	CWords& operator= (const CWords& cwords);

	bool is_end() { return end_; }

	int pos() { return pos_; }

	const std::list<word_ptr_type>& words() const { return cwords_; }
	void append(const_word_ptr w_ptr);
	void insert(const_word_ptr w_ptr);

	double cal(Freq* freq);
	void add_reward_or_penalty(Freq* freq);
	double score() { return score_; }

	array_type to_array();
	string_type	to_string();
	int size() { return cwords_.size(); }

	/// the size of cwords_ cann't go beyond a limit, for example,
	/// a English frequency pattern string could not
	/// be separated into greater than n/2 words due to the fact of English phrase formation
	bool reach_limit();
	static double h_score() { return highest_s_; }
	static double l_score() { return lowest_s_; }
	static void h_score(double s) { highest_s_ = s; }
	static void l_score(double s) { lowest_s_ = s; }

	int chinese_stop_word_count();

private:
	void copy(const CWords& cwords);
	void adjust_pos(const_word_ptr w_ptr);

	void cal_harmonic_mean(Freq* freq);
	void cal_geometric_mean(Freq* freq);
	void cal_product(Freq* freq);
	void cal_reverse_mi(Freq* freq); // calculate reverse mutual information
	void cal_reverse_mi2(Freq* freq);
	void cal_mi(Freq* freq, int switcher = 0); // calculate mutual information
	void get_reward_or_penalty(Freq* freq);

};


#endif /*CWORDS_H_*/
