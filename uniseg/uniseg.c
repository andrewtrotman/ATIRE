/*
 * UNISEG_PLUGIN.C
 *
 *  Created on: Jul 10, 2009
 *      Author: monfee
 */

#include "uniseg.h"
#include "seg.h"
#include "qfreq.h"
#include "encoding_factory.h"
#include "uniseg_settings.h"
#include "utilities.h"

#include <iostream>

using namespace std;

UNISEG_uniseg::UNISEG_uniseg() : uniseg_plugin_interface()
{
    //output_ = 0;
    count_ = 0;
    //seger = NULL;
    enc_ = UNISEG_encoding_factory::instance().get_encoding();

    // load the string frequency table
    cerr << "##################### initializing UNISEG segmentation module ######################" << endl;
    //seger.load_frqs();
    QFreq::instance().load_freq();
    cerr << "############################# finished initialization ##############################" << endl;
}

UNISEG_uniseg::~UNISEG_uniseg()
{
    cleanup();
}

void UNISEG_uniseg::cleanup()
{
}

const unsigned char *UNISEG_uniseg::do_segmentation(unsigned char *c, int length)
{
	if (!c || strlen((char *)c) == 0 || length == 0)
		return NULL;

	/**
	 * segment a few characters a time
	 */
	unsigned char *current = c;
	unsigned char *next = current;
	unsigned char *end = c + length;
	const long step = UNISEG_settings::instance().max;
	long segmented_len = 0;
	long how_far = 0;
	long distance = 0;
	long count = 0;
	bool in_middle_flag = false;
	bool stop = false;
	bool need_eligibility_check = QFreq::instance().need_eligibility_check();
	output_.clear();
	count_ = 0;

	while (current < end) {
		count = 0;
		in_middle_flag = false;

		if (output_.length() > 0)
			output_.append("  ");

		while (current < end && isspace(*current))
			output_.push_back(*current++);

		next = current;
		if (next < end) {
			enc_->test_char(next);
			if (enc_->lang() == uniseg_encoding::CHINESE)
				while (next < end && enc_->lang() == uniseg_encoding::CHINESE /*&& count < step*/) {
					next += enc_->howmanybytes();
					enc_->test_char(next);
					count++;
				}

			if (enc_->lang() == uniseg_encoding::CHINESE)
				in_middle_flag = true;

			if (count <= 0) {
				//break;

//			if (*next & 80) {
//				current = next += enc_->howmanybytes();
//				enc_->test_char(next);
//				while (next < end && enc_->lang() != uniseg_encoding::CHINESE && enc_->lang() != uniseg_encoding::SPACE) {
//					next += enc_->howmanybytes();
//					output_.append((const char *)current, enc_->howmanybytes());
//					enc_->test_char(next);
//					current = next;
//				}
//			}
//			else {
				string_array temp_word_array;
				unsigned char *pre = next;
				next += enc_->howmanybytes();
				temp_word_array.push_back(string_type(pre, next));
				enc_->test_char(next);

				while (next < end && enc_->lang() != uniseg_encoding::CHINESE && enc_->lang() != uniseg_encoding::SPACE) {
					pre = next;
					next += enc_->howmanybytes();
					temp_word_array.push_back(string_type(pre, next));
					enc_->test_char(next);
				}

				while (next < end && enc_->lang() == uniseg_encoding::CHINESE) {
					pre = next;
					next += enc_->howmanybytes();
					temp_word_array.push_back(string_type(pre, next));
					string_type word_str;
					arraytostring(temp_word_array, word_str);
					if (!QFreq::instance().fuzzy_search_dic(word_str)) {
						temp_word_array.pop_back();
						break;
					}
					enc_->test_char(next);
				}
				while (temp_word_array.size() > 1 && (*temp_word_array.back().c_str() & 0x80)) {
					string_type word_str;
					arraytostring(temp_word_array, word_str);
					if (!QFreq::instance().is_word(word_str))
						temp_word_array.pop_back();
					else
						break;
				}
				string_type non_all_chinese_word;
				arraytostring(temp_word_array, non_all_chinese_word);
				if (non_all_chinese_word != "\357\273\277")
					output_.append(non_all_chinese_word);
				current += non_all_chinese_word.length();
				continue;
			}
		}

		how_far = next - current;
		string_type input((char *)current, how_far);
		if (input == "考虑到日本债务水平已然相当于国内生产总值")
			cerr << "stop here" << endl;
	    if (how_far > 0 && count > 1) {
	    	if (/*count <= QFreq::instance().freq_training().array_size() && */!QFreq::instance().is_word(input)) {
				seger_.input(input);
				seger_.start();
				std::vector<double>& boundary_score = seger_.boundary_score();
				const array_type& words_list = seger_.best_words();
				long word_count = 0;
				long i = 0;
				long size = (in_middle_flag && words_list.size() > 1) ? words_list.size() - 1 : words_list.size();
				if (size > 0)
					for (; i < size; i++) {
						word_ptr_type current_word = words_list[i];
						bool has_word_pair = current_word->has_word_pair() && !current_word->is_word();
						if (i > 0 /*&& *(output_.end()--) != ' '*/)
							output_.append("  ");
						if (current_word->size() == 1
								|| has_word_pair
								|| current_word->is_word()
								|| (need_eligibility_check && QFreq::instance().eligibility_check(current_word))) {
							string_type& word = current_word->chars();
							if (has_word_pair)
								output_.append(current_word->left()->chars() + "  " + current_word->right()->chars());
							else
								output_.append(word);
							segmented_len += word.length();
						}
						else {
							/*
							 * Check if it is OOV
							 */
							if (!current_word->is_candidate_word())
								QFreq::instance().freq_text()->check_oov(current_word, UNISEG_settings::instance().oov_threshold);
							string to_become;
							if (current_word->is_candidate_word())
								to_become = current_word->chars();
							else {
								/* 2010.06.03 */
								std::vector<double>::iterator begin = boundary_score.begin() + word_count;
								std::vector<double>::iterator end = begin + (current_word->size() - 1);
								seger_.find_boundary(begin, end, current_word, to_become);
//								seger_.get_rightmost_word_segmentation(current_word, to_become);
							}
							cerr << current_word->chars() << " > " << to_become << endl;
							output_.append(to_become);
							segmented_len += current_word->chars().length();
						}
						word_count +=  current_word->size();
					}
				else {
					segmented_len = how_far;
					output_.append(string_type(current, current + how_far) + "  ");
				}
				// put the last segment back to the remaining characters
				current += segmented_len;
				segmented_len = 0;
				count_ += (words_list.size() == size) ? 0 : count - words_list[size - 1]->size();
				continue;
			}
	    }

		//how_far = how_far > 0 ? how_far : enc_->howmanybytes();
		output_.append(input);
		current += how_far;
	}

	return get_output();
}

const unsigned char *UNISEG_uniseg::get_input()
{
	return (unsigned char *)input_.c_str();
}

const unsigned char *UNISEG_uniseg::get_output()
{
	return (unsigned char *)output_.c_str();
}

int UNISEG_uniseg::get_count() { return count_; }


#ifdef  __cplusplus
extern "C" {
#endif
	uniseg_plugin_interface *maker() {
	   return new UNISEG_uniseg;
	}

#ifdef WITH_ANT_PLUGIN
	class proxy
	{
	public:
	   proxy() {
		   std::cerr << "Registering UNISEG to the plugin factory..." << std::endl;
		  // register the maker with the factory
		   plugin_factory[uniseg_plugin_interface::SEGMENTATION].maker = maker;
	   }
	};
	// our one instance of the proxy
	proxy p;
#endif

#ifdef  __cplusplus
}
#endif
