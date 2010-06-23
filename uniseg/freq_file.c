/*
 * freq_file.cpp
 *
 *  Created on: Oct 26, 2008
 *      Author: monfee
 */

#include "uniseg_settings.h"
#include "freq.h"
#include "freq_file.h"
#include "utilities.h"
#include "encoding_factory.h"

#include <string.h>
#include <iostream>
#include <cassert>

#include <stdio.h>

using namespace std;

/*
FreqFile::FreqFile(std::string name)
	: File(name) {

}

FreqFile::FreqFile(std::string name, unsigned int size)
	: File(name, size) {

}
*/


FreqFile::~FreqFile() {

}

void FreqFile::init() {
	EXT_NAME = "frq";
	wlen_ = 0;
	File::setup();
	loaded_ = false;
}

unsigned int FreqFile::cal(array_type& arr) {
	//if (arr[0]->address() == INVALID)
	//	freq_->alloc(k);
	//File::word_ptr_type last_word = freq_n_[k - 1][freq_n_[k - 1].size() - 1];
	/*
	for (int j = 0; j < freq_n_[k - 1].size(); j++)
		cerr << "last word: " << freq_n_[k - 1][j]->chars()
			<< " address: " << freq_n_[k - 1][j]->address()
			<< " freq: " << freq_n_[k - 1][j]->freq()<< endl;
			*/

	//t_size = last_word->address()
	//	+ last_word->freq() * 2 * 8/*(k -1)*/
	//	+ freq_n_[k - 1].size() * 8;
	return (UNICODE_CHAR_LENGTH + sizeof(int)) * arr.size();
}

void FreqFile::write(array_type& arr) {

	if (arr.size() <= 0)
		return;

	array_type should_save_words;

	File::wopen();

	if(!iofs_) {
	    cerr << "Cannot open file for writing.\n";
	    return;
	}
	cerr << "expected total size: " << cal(arr) << endl;

	iofs_.seekp(0, ios::beg);

	string_type pre_parent;
	int count = 0;

	for (unsigned int i = 0; i < arr.size(); i++) {
		word_ptr_type word_ptr = arr[i];

		if (UNISEG_settings::instance().on_training && (!word_ptr->has_word_pair() && !word_ptr->is_word()))
			continue;

		unsigned int freq = word_ptr->freq();

		if (/*UNISEG_settings::instance().do_skip && */freq < UNISEG_settings::instance().to_skip)
			continue;

		char *chars = (char *)(word_ptr->family().second->chars().c_str());

		int len = strlen(chars);
		assert(len <= UNICODE_CHAR_LENGTH);

		iofs_.write(chars, UNICODE_CHAR_LENGTH);
		unsigned int word_where = 0;
		if (word_ptr->is_word()
				|| (word_ptr->left() != NULL && word_ptr->right() != NULL && word_ptr->left()->is_word() && word_ptr->right()->is_word())) {
			if (word_ptr->is_word()) {
//				if (word_ptr->chars() == "\344\270\232\345\214\226\344\270\213")
//					cerr << "stop here" << endl;
				word_where = 0xFF;
			}
			else {
				word_where = word_ptr->left()->size();
				assert(word_where <= word_ptr->size());
			}
			char tmp[INT_TYPE_SIZE];
			int32_to_bytes(freq, tmp);

			tmp[INT_TYPE_SIZE - 1] = word_where;
			iofs_.write(tmp, INT_TYPE_SIZE);
		}
		else
			iofs_.write((char *)&freq, sizeof(unsigned int));

		should_save_words.push_back(word_ptr);
		++count;
	}

	ofstream::pos_type pos = iofs_.tellp();
	cerr << "number of word: " << arr.size() << endl;
	cerr << "number of saved word: " << count << endl;
	//cerr << "expected file size: " << size << endl;
	cerr << "actual file size: " << static_cast<int>(pos) << endl;
	//assert(size == static_cast<int>(pos));

	iofs_.close();

	// writh the index file
	if (should_save_words.size() > 0) {
		if (wlen_ > 1) {
			idxf_.path(path_);
			idxf_.wlen(wlen_);
			idxf_.alloc(should_save_words);
			idxf_.write();
		}
	}
	else
		remove(filename_.c_str());
}

void FreqFile::read() {
	if (loaded_)
		return;
	cerr << "loading " << filename_ << endl;

	char buf[UNICODE_CHAR_LENGTH] = {0x0, 0x0, 0x0, 0x0};
	char tmp[INT_TYPE_SIZE];
	unsigned int  value;

	assert(wlen_ == 1);

//	File::ropen();
//	File::read_in_memory();
	char *current = buf_;
	uniseg_encoding *enc = uniseg_encoding_factory::instance().get_encoding();

//	if (!iofs_) {
//		// An error occurred!
//		// iofs_.gcount() returns the number of bytes read.
//		// calling iofs_.clear() will reset the stream state
//		// so it is usable again.
//		cerr << "unable to open file \"" << name_ << "\"" << endl;
//	} else {
		int count = 0;

		while (count < size_) {
//			if (!iofs_.read (buf, UNICODE_CHAR_LENGTH)
//					|| !iofs_.read ((char *)&value, sizeof(unsigned int))) {
//				// Same effect as above
//				cerr << "Errors when reading file \"" << name_ << "\"" << endl;
//			}
			memcpy(buf, current, UNICODE_CHAR_LENGTH);
			current += UNICODE_CHAR_LENGTH;
			memcpy(tmp, current, INT_TYPE_SIZE);
			//value = bytes_to_int32(tmp);
			current += INT_TYPE_SIZE;

			string_array ca;
			/// save them in the array
			enc_->test_char((unsigned char*)&buf);
			string_type a_char(buf, enc_->howmanybytes());
			ca.push_back(a_char);

//			freq_->add(ca, enc_->lang(), value); //->address(count/RECORD_LENGTH);
			add_word(ca, tmp);
			count += RECORD_LENGTH;
			// calculating the total characters appear in the corpus
			//if (k == 1)
			//	num += value;
		}

		cerr << "loaded number of word with size(" << wlen_ << ") : " << freq_->array_k_size(wlen_) << endl;
		//if (k == 1)
		//	cerr << "total characters: " << num << " approximily " << num*3/(1024*1024) << "m" << endl;
//	}
	iofs_.close();
	loaded_ = true;
}

void FreqFile::read_with_index() {
	if (loaded_)
		return;

	cerr << "loading " << name_ << endl;

	char buf[UNICODE_CHAR_LENGTH] = {0x0, 0x0, 0x0, 0x0};
	char tmp[INT_TYPE_SIZE];
	unsigned int  value = 0;
	char *current = buf_;

	IndexFile idxf(name_);
	idxf.path(path_);
	idxf.wlen(wlen_);
	idxf.read();
//	File::ropen();
	File::read_in_memory();

	if (!iofs_) {
		// An error occurred!
		// iofs_.gcount() returns the number of bytes read.
		// calling iofs_.clear() will reset the stream state
		// so it is usable again.
		cerr << "unable to open file \"" << filename_ << "\"" << endl;
	} else {
		int count = 0;

		for (unsigned int i = 0; i < idxf.aa().size(); i++) {
			string_array& ca = idxf.aa()[i].ca();
			//std::copy(ca.begin(), ca.end(), idxf.aa()[i].ca().begin());

			unsigned int llb = idxf.aa()[i].bound()[0];
			unsigned int lhb = 0, rhb = 0;
			unsigned int rlb = idxf.aa()[i].bound()[1];

			fstream::pos_type pos = 0;
			if ( rlb != Address::INVALID_BOUND) {
				lhb = rlb;

				if (llb != Address::INVALID_BOUND)
					rlb = lhb + 1;

				rhb = idxf.aa()[i].bound()[2];
			} else
				lhb = idxf.aa()[i].bound()[2];

			if (llb != Address::INVALID_BOUND) {
				pos = llb * RECORD_LENGTH;
				current = buf_ + pos;
//				iofs_.seekg(pos, ios::beg);
//				assert(pos < size_);

				for (unsigned int j = llb; j <= lhb && j != Address::INVALID_BOUND; j++) {
					memcpy(buf, current, UNICODE_CHAR_LENGTH);
					current += UNICODE_CHAR_LENGTH;
					memcpy(tmp, current, INT_TYPE_SIZE);
					value = bytes_to_int32(tmp);
					current += INT_TYPE_SIZE;

					/// save them in the array
					enc_->test_char((unsigned char*)&buf);
					string_type a_char(buf, enc_->howmanybytes());
					string_array aca(ca);
					aca.push_back(a_char);
					assert(value >= 0);
					freq_->add(aca, enc_->lang(), value); //->address(count);
					count++;
				}
			}

			if (rlb != Address::INVALID_BOUND) {
				pos = rlb * RECORD_LENGTH;
				current = buf_ + pos;
//				iofs_.seekg(pos, ios::beg);
//				assert(pos < size_);

				for (unsigned int j = rlb; j <= rhb && j != Address::INVALID_BOUND; j++) {

					memcpy(buf, current, UNICODE_CHAR_LENGTH);
					current += UNICODE_CHAR_LENGTH;
					memcpy(tmp, current, INT_TYPE_SIZE);
					value = bytes_to_int32(tmp);
					assert(value >= 0);
					current += INT_TYPE_SIZE;

					/// save them in the array
					enc_->test_char((unsigned char*)&buf);
					string_type a_char(buf, enc_->howmanybytes());
					string_array aca = ca;
					aca.insert(aca.begin(), a_char);
					freq_->add(aca, enc_->lang(), value); //->address(count);
					count++;
				}
			}
		}

		cerr << "loaded number of word with size(" << wlen_ << ") : " << freq_->array_k_size(wlen_) << endl;
		cerr << "actual number of word with size(" << wlen_ << ") : " << count << endl;
		//if (k == 1)
		//	cerr << "total characters: " << num << " approximily " << num*3/(1024*1024) << "m" << endl;
	}

	iofs_.close();
	loaded_ = true;
}

void FreqFile::read_term(word_ptr_type word)
{
	if (word->disk_address().size() == 0 || word->is_loaded())
		return;

	Address::uint_array& address = word->disk_address();
	string_array ca;
	word->to_string_array(ca);
	char *current = NULL;
	char buf[UNICODE_CHAR_LENGTH] = {0x0, 0x0, 0x0, 0x0};
	char tmp[INT_TYPE_SIZE];

	int count = 0;

	uniseg_encoding *enc = uniseg_encoding_factory::instance().get_encoding();

	unsigned int llb = address[0];
	unsigned int lhb = 0, rhb = 0;
	unsigned int rlb = address[1];

	fstream::pos_type pos = 0;
	if ( rlb != Address::INVALID_BOUND) {
		lhb = rlb;

		if (llb != Address::INVALID_BOUND)
			rlb = lhb + 1;

		rhb = address[2];
	} else
		lhb = address[2];

	if (llb != Address::INVALID_BOUND) {
		pos = llb * RECORD_LENGTH;
		current = buf_ + pos;
	//				iofs_.seekg(pos, ios::beg);
	//				assert(pos < size_);

		// load word that character is on the right side of the this word
		for (unsigned int j = llb; j <= lhb && j != Address::INVALID_BOUND; j++) {
			memcpy(buf, current, UNICODE_CHAR_LENGTH);
			current += UNICODE_CHAR_LENGTH;

			enc_->test_char((unsigned char*)&buf);
			string_type a_char(buf, enc_->howmanybytes());
			string_array aca(ca);
			aca.push_back(a_char);
			// debug
		//			if (word->chars() == "\347\272\246" && a_char == "\346\227\246")
		//				cerr << "I got you" << endl;

			memcpy(tmp, current, INT_TYPE_SIZE);
			current += INT_TYPE_SIZE;

			add_word(aca, tmp);
			//cerr << "add new word: " << ret_word->chars() << " " << ret_word->freq() << endl;
			count++;
		}
	}

	if (rlb != Address::INVALID_BOUND) {
		pos = rlb * RECORD_LENGTH;
		current = buf_ + pos;
	//				iofs_.seekg(pos, ios::beg);
	//				assert(pos < size_);

		// load word that character is on the left side of the this word
		for (unsigned int j = rlb; j <= rhb && j != Address::INVALID_BOUND; j++) {

			memcpy(buf, current, UNICODE_CHAR_LENGTH);
			current += UNICODE_CHAR_LENGTH;
			/// save them in the array
			enc_->test_char((unsigned char*)&buf);
			string_type a_char(buf, enc_->howmanybytes());
			string_array aca = ca;
			aca.insert(aca.begin(), a_char);

			memcpy(tmp, current, INT_TYPE_SIZE);

			//value = bytes_to_int32(tmp);
			//assert(value > 0);
			current += INT_TYPE_SIZE;

			add_word(aca, tmp);
			//word_ptr_type ret_word = freq_->add(aca, enc_->lang(), value); //->address(count);
			//cerr << "add new word: " << ret_word->chars() << " " << ret_word->freq() << endl;
			count++;
		}
	}
	word->set_loaded(true);
}

void FreqFile::add_word(string_array& aca, char *freq_bytes)
{
	/*
	 * the last byte is used for indicating where is the stop of a word
	 */
	unsigned char word_where = (unsigned char)freq_bytes[3];
	freq_bytes[INT_TYPE_SIZE - 1] = 0;

	unsigned int  value = bytes_to_int32(freq_bytes);
	if (value > 16777000)
		cerr << "Unbelievable." << endl;

	/// save them in the array
	// assert(value > 0);
	word_ptr_type ret_word = freq_->add(aca, enc_->lang(), value); //->address(count);
	freq_->add_to_array(ret_word);

	if (ret_word->chars().find("密林") != string::npos)/* || ret_word->chars() == "\347\272\242\347\201\257\347\254\274"*/
		cerr << "got you " << endl;

	if (word_where == 0xFF) {
		ret_word->is_word(true);
		ret_word->adjust(ret_word->freq(), true);
	}
	else if (word_where > 0) {
		assert(ret_word->size() > 1);
		string left = ret_word->subchars(0, word_where);
		string right = ret_word->subchars(word_where, ret_word->size() - word_where);

		word_ptr_type a_word = freq_->find(left);
		assert(a_word != NULL);
		ret_word->left(a_word);
		a_word->is_word(true);
		int len = a_word->size();

		a_word = freq_->find(right);
		assert(a_word != NULL);
		ret_word->right(a_word);
		a_word->is_word(true);
		len += a_word->size();

		assert(ret_word->size() == len);
	}
}

void FreqFile::load_index()
{
	if (wlen_ > 1) {
		idxf_.name(name_);
		idxf_.path(path_);
		idxf_.wlen(wlen_);
		idxf_.read(freq_);
	}
}

void FreqFile::load()
{
//	array_type& word_array = freq_->array_k(wlen_ - 1);
//	for (int i = 0; i < word_array.size(); ++i)
//		read_term(word_array[i]);

	freq_type& word_map = freq_->set();
	for (freq_type::iterator it = word_map.begin(); it != word_map.end(); ++it) {
		if (it->second->size() == (wlen_ - 1))
			read_term(it->second);
	}
}
