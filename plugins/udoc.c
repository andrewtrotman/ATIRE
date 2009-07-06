/*
 * udoc.cpp
 *
 *  Created on: Jan 28, 2009
 *      Author: monfee
 */


#include "udoc.h"
#include "qfreq.h"
#include "uniseg_settings.h"
#include "convert.h"

using namespace std;

//std::string UDoc::EXT_NAME = "txt";

UDoc::UDoc(Doc* doc, std::set<string_type>* seged_ptr) : doc_(doc)
	, counter_(&freq_), seged_ptr_(seged_ptr), File::File() {

	init();
}

UDoc::~UDoc(){
	if (iofs_.is_open())
		iofs_.close();

	clear();
}

void UDoc::init() {

/*	doc_->reset();
	while (doc_->more()) {
		entity_iterator begin;
		entity_iterator end;

		doc_->next(begin, end);

		//sents_.push_back(string_type());
		string_type sent;

		while (begin <= end) {
			string_type str = (*begin)->to_string();
			if (UNISEQ_settings::instance().lang() == stpl::ENGLISH) {
				tolower(str);
			}
			//sents_[sents_.size() - 1].append(str);
			sent.append(str);
			begin++;
		}

		cout << sent << endl;
	}*/

	if (UNISEQ_settings::instance().do_save()) {
		this->EXT_NAME = "txt";
		name(doc_->name());
		path(UNISEQ_settings::instance().opath() + string(UNISEQ_settings::instance().separator()));
		wopen();
	}
}

void UDoc::clear() {
	marks_map::const_iterator iter;
    for (iter=mmap_.begin(); iter != mmap_.end(); ++iter)
    	delete iter->first;
	mmap_.clear();
}

void UDoc::seg(){

	/// build table
	array_type& wa = build_freq_table();

	/*for (int i = 0; i < sents_.size(); i++) {
		string_type& sent = sents_[i];
		int slen = sent.length();*/
	doc_->reset();
	while (doc_->more()) {
		entity_iterator begin;
		entity_iterator end;

		doc_->next(begin, end);
		int slen = end - begin + 1;
		int tlen = 0;
		//bool go_next = false;

		// clear all the assistance map and array
		clear();
		fil_init(slen);


		for (int j = 0; j < wa.size(); j++) {
			if (wa[j]->seged() /*|| wa[j]->sjze() == 1*/)
				continue;
			if (wa[j]->freq() < 2)
				continue;

			string_type::size_type pos;
			string_type str = wa[j]->chars();
			unsigned int str_len = wa[j]->size(); //str.length();

			// debug
			//if (str == "usandm" || str == "andmy" /*|| str == "art" || str == "com"*/)
			//	cout << " I got " << str << endl;

			//pos = sent.find(str);
			pos = 0;
			//while ((pos = sent.find(str, pos)) != string_type::npos) {
			while ((pos = find(wa[j], begin, end, pos)) != -1/*string_type::npos*/) {
				unsigned int left = pos;
				unsigned int right = pos + str_len - 1;

				pos += str_len;
				string_type ins_str = str;
				unsigned int fil_len = str_len;
				/// check if there are empty slots
				if (empty_check(left, right)) {
					if ((slen - right - 1) > 0) {
						// a character backward to make sure that not taking the character from
						// the following word
						// with conditions:
						// 1. no overlapping
						// 2. it is not the last word in a sentence

						word_ptr_type w_ptr = wa[j]->lparent();
						if (w_ptr != NULL && w_ptr->freq() > 1) {
							ins_str = w_ptr->chars();
							right--;
							fil_len--;
						}
					}
				}
				else{
					/// check if the word has been marked yet
					if (mark_check(left, right))
						continue;
				}

				Range* range_ptr = new Range(left, right);
				range_ptr->lo(ec_reg_.first);
				range_ptr->ro(ec_reg_.second);

				mmap_.insert(make_pair(range_ptr, find(ins_str)));
				tlen += fill(left, fil_len);

				if (tlen >= slen) {
					break;
				}

			}

			if (tlen >= slen) {
				break;
			}
		}

		/// resolve the conflicts
		resolve_conflict();

		/// find missing
		if (tlen < slen)
			find_missing(begin, end);

		/// show for temporary
		show();
		if (UNISEQ_settings::instance().do_save())
			save();
		collect();
	}

}

array_type& UDoc::build_freq_table() {
	counter_.count(*doc_, QFreq::instance().count(), 1);

	//counter.assign_array();
	//counter.remove_array_r();
	//counter.show_array();

	counter_.overall(QFreq::instance().freq());
	return counter_.wa();
}

/**
 * @return true for marked
 */
bool UDoc::mark_check(unsigned int left, unsigned int right) {
	assert ((ec_reg_.first && ec_reg_.second) || ec_reg_.first || ec_reg_.second);

	/// double overlapping or within a word
	if (ec_reg_.first && ec_reg_.second)
		return true;

	marks_map::const_iterator iter;
    for (iter=mmap_.begin(); iter != mmap_.end(); ++iter) {

    	// for debug
    	word_ptr_type w_ptr = iter->second;
    	const string_type str = w_ptr->chars();
    	Range* range_ptr = iter->first;

    	/// because when come to this function that means empty check failed
    	/// the word must overlap with one in the map
    	///  1. if there is another word overlapping with the current node, it is marked
    	///  2. it cannot overlap with two other words

    	// check if overlap, if not, look for next one
    	/// if ec_reg_.second true means left overlapping
    	/// if ec_reg_.first true means right overlapping
    	if (
    			(/*ec_reg_.second &&*/ (range_ptr->right() < left))
    			|| (ec_reg_.second && range_ptr->lo())
    			|| (ec_reg_.first && range_ptr->ro())
    			)
    		continue;

    	assert(right >= range_ptr->left());

    	/// overlapping, to see if overlapping with another one
    	/// not necessary to check as below, because if ec_reg_.first & ec_reg_.second
    	/// all true then means double overlapping
/*    	marks_map::const_iterator next = iter;
    	next++;
    	if (next != mmap_.end()) {
    		Range* next_range = next->first;
    		string_type& next_str = next->second;
    		if (next_range->left < right)
    			return true; // already marked
    	}*/

    	/*if (range_ptr->left() <= left
    			&& range_ptr->right() >= right)
    		return true;*/

    	int osl, osr;

    	//if (!ec_reg_.first)
    	osl = range_ptr->left() - left;
    	osr = right - range_ptr->right();

    	/// left overlapping for this node, but right overlapping for new range
    	if (osl > 1 && !ec_reg_.first) {
    		range_ptr->lo(true);
    		return false;
    	}

    	/// right overlapping for this node, but left overlapping for new range
    	if (osr > 1 && !ec_reg_.second) {
    		range_ptr->ro(true);
    		return false;
    	}
    	return true;
    }
    return true;
}

/**
 * @return false not empty
 */
bool UDoc::empty_check(unsigned int left, unsigned int right) {
	ec_reg_.first = fil_[left];
	ec_reg_.second = fil_[right];
	return (!ec_reg_.first) && (!ec_reg_.second);
}

void UDoc::fil_init(unsigned int len) {
	fil_.clear();
	assert(fil_.size() == 0);
	for (int i = 0; i < len; i++) {
		fil_.push_back(false);
	}
}

unsigned int UDoc::fill(unsigned int left, unsigned int len) {
	unsigned int fil_len = 0;
	for (int i = 0; i < len; i++) {
		int idx = left + i;
		if (!fil_[idx]) {
			fil_len++;
			fil_[idx] = true;
		}
	}
	return fil_len;
}

void UDoc::show() {
	marks_map::const_iterator iter;
    for (iter=mmap_.begin(); iter != mmap_.end(); ++iter) {
    	cout << iter->second->chars() << " ";
    }
    cout << endl;
}

void UDoc::resolve_conflict() {
	marks_map::iterator iter;
    for (iter=mmap_.begin(); iter != mmap_.end(); /*++iter*/) {

    	// for debug
    	word_ptr_type fw_ptr = iter->second;
    	const string_type fstr = fw_ptr->chars();
    	Range* frange_ptr = iter->first;

		// debug
		//if (fstr == "and" || fstr == "andmy" || fstr == "religious" /*|| str == "com"*/)
		//	cout << " I got " << fstr << endl;

    	marks_map::iterator next = iter;
    	next++;

    	if (frange_ptr->ro()) {
			if (next != mmap_.end()) {
				word_ptr_type sw_ptr = next->second;
				const string_type sstr = sw_ptr->chars();
				Range* srange_ptr = next->first;

				assert(srange_ptr->lo());

				int ol_n = frange_ptr->right() - srange_ptr->left() + 1;
				assert(ol_n > 0);

				int sub_len = frange_ptr->right() - frange_ptr->left() + 1 - ol_n;

				word_ptr_type found_w_ptr;
				if (sub_len > 0)
					found_w_ptr = fw_ptr->subword(0, sub_len);

				// sub_len must be greater than 0,
				// otherwise the whole current node is a part of the next word
				if (sub_len > 0 && found_w_ptr->freq() > UNISEQ_settings::instance().to_skip()) {
					iter->second = found_w_ptr;
					frange_ptr->right(frange_ptr->right() - ol_n);
				} else {
					sub_len = srange_ptr->right() - srange_ptr->left() + 1 - ol_n;
					found_w_ptr = sw_ptr->subword(ol_n, sub_len);

					next->second = found_w_ptr;
					srange_ptr->left(srange_ptr->left() + ol_n);
				}
			}
    	}
    	iter = next;
    }
}

void UDoc::find_missing(const entity_iterator begin, const entity_iterator end) {
	int len = 0;
	int fsize = fil_.size();
	int slen = end - begin + 1;
	assert(fsize == slen);

	for (int i = 0; i < fil_.size(); i++) {
		if (!fil_[i]) {
			int idx = i;
			string_type substr;

			do {
				len++;
				string_type a_char = (*(begin + i))->to_string();
				if (UNISEQ_settings::instance().lang() == stpl::ENGLISH) {
					tolower(a_char);
				}
				substr.append(a_char);
				i++;
			} while((!fil_[i]) && (i < fil_.size()));
			// = str.substr(idx, len);
			mmap_.insert(make_pair(new Range(idx, idx + len -1), find(substr)));
			len = 0;
		}
	}
}

/**
 * TODO -- to implement a more efficient search algorithm,
 * 		the initial search algorithm is a naive search one
 */
int UDoc::find(word_ptr_type w_ptr
							, entity_iterator begin
							, entity_iterator end
							, unsigned int pos) {
	const array_type& ca = w_ptr->array();

	unsigned int slen = end - begin + 1;
	unsigned int psize = ca.size();
	if (slen < psize || (slen - pos) < psize)
		return -1;

	entity_iterator next = begin + pos;
	entity_iterator stop = end - psize + 1;

	while (next <= stop) {
		int i = 0;

		for (; i < psize; i++) {
			entity_iterator cur = next + i;

			string_type str = (*cur)->to_string();
			if (UNISEQ_settings::instance().lang() == stpl::ENGLISH) {
				tolower(str);
			}
			if (ca[i]->chars() != str)
				break;
		}

		if (i == psize)
			return next - begin;
		next++;
	}
	return -1;
}

word_ptr_type UDoc::find(string_type str) {
	word_ptr_type w_ptr = freq_.find(str);
	assert(w_ptr != NULL);
	return w_ptr;
}

void UDoc::collect() {
	marks_map::const_iterator iter;
    for (iter=mmap_.begin(); iter != mmap_.end(); ++iter) {
    	string_type word = iter->second->chars();
    	if (!(seged_ptr_->find(word) != seged_ptr_->end()))
    		seged_ptr_->insert(word);
    }
}

void UDoc::save() {
	if (iofs_.is_open()) {
		marks_map::const_iterator iter;
	    for (iter=mmap_.begin(); iter != mmap_.end(); ++iter) {
	    	iofs_ << iter->second->chars() << " ";
	    }
	    iofs_ << endl;
	}
}
