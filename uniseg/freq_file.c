/*
 * freq_file.cpp
 *
 *  Created on: Oct 26, 2008
 *      Author: monfee
 */

#include "uniseg_settings.h"
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
	//	freq_.alloc(k);
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

		char *chars;
		word_ptr_type word_ptr = arr[i];
		unsigned int freq = 0;

		chars = (char *)(word_ptr->family().second->chars().c_str());

		int len = strlen(chars);
		assert(len <= UNICODE_CHAR_LENGTH);

		freq = word_ptr->freq();
		if (UNISEG_settings::instance().do_skip && freq <= UNISEG_settings::instance().to_skip)
			continue;

		iofs_.write(chars, UNICODE_CHAR_LENGTH);
		if (word_ptr->lparent()->is_word() && word_ptr->rparent()->is_word()) {
			char tmp[INT_TYPE_SIZE];
			int32_to_bytes(freq, tmp);
			tmp[INT_TYPE_SIZE - 1] = word_ptr->lparent()->size();
			iofs_.write(tmp, INT_TYPE_SIZE);
		}
		else
			iofs_.write((char *)&freq, sizeof(unsigned int));
		++count;
	}


	ofstream::pos_type pos = iofs_.tellp();
	cerr << "number of word: " << arr.size() << endl;
	cerr << "number of saved word: " << count << endl;
	//cerr << "expected file size: " << size << endl;
	cerr << "actual file size: " << static_cast<int>(pos) << endl;
	//assert(size == static_cast<int>(pos));

	iofs_.close();
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
			strncpy(buf, current, UNICODE_CHAR_LENGTH);
			current += UNICODE_CHAR_LENGTH;
			strncpy(tmp, current, INT_TYPE_SIZE);
			value = bytes_to_int32(tmp);
			current += INT_TYPE_SIZE;

			string_array ca;
			/// save them in the array
			enc_->test_char((unsigned char*)&buf);
			string_type a_char(buf, enc_->howmanybytes());
			ca.push_back(a_char);

			freq_.add(ca, enc_->lang(), value); //->address(count/RECORD_LENGTH);
			count += RECORD_LENGTH;
			// calculating the total characters appear in the corpus
			//if (k == 1)
			//	num += value;
		}

		cerr << "loaded number of word with size(" << wlen_ << ") : " << freq_.array_k_size(wlen_) << endl;
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
					strncpy(buf, current, UNICODE_CHAR_LENGTH);
					current += UNICODE_CHAR_LENGTH;
					strncpy(tmp, current, INT_TYPE_SIZE);
					value = bytes_to_int32(tmp);
					current += INT_TYPE_SIZE;

					/// save them in the array
					enc_->test_char((unsigned char*)&buf);
					string_type a_char(buf, enc_->howmanybytes());
					string_array aca(ca);
					aca.push_back(a_char);
					assert(value >= 0);
					freq_.add(aca, enc_->lang(), value); //->address(count);
					count++;
				}
			}

			if (rlb != Address::INVALID_BOUND) {
				pos = rlb * RECORD_LENGTH;
				current = buf_ + pos;
//				iofs_.seekg(pos, ios::beg);
//				assert(pos < size_);

				for (unsigned int j = rlb; j <= rhb && j != Address::INVALID_BOUND; j++) {

					strncpy(buf, current, UNICODE_CHAR_LENGTH);
					current += UNICODE_CHAR_LENGTH;
					strncpy(tmp, current, INT_TYPE_SIZE);
					value = bytes_to_int32(tmp);
					assert(value >= 0);
					current += INT_TYPE_SIZE;

					/// save them in the array
					enc_->test_char((unsigned char*)&buf);
					string_type a_char(buf, enc_->howmanybytes());
					string_array aca = ca;
					aca.insert(aca.begin(), a_char);
					freq_.add(aca, enc_->lang(), value); //->address(count);
					count++;
				}
			}
		}

		cerr << "loaded number of word with size(" << wlen_ << ") : " << freq_.array_k_size(wlen_) << endl;
		cerr << "actual number of word with size(" << wlen_ << ") : " << count << endl;
		//if (k == 1)
		//	cerr << "total characters: " << num << " approximily " << num*3/(1024*1024) << "m" << endl;
	}

	iofs_.close();
	loaded_ = true;
}

void FreqFile::read_term(word_ptr_type word)
{
	if (word->is_loaded())
		return;

	Address::uint_array& address = word->disk_address();
	string_array ca;
	word->to_string_array(ca);
	char *current = NULL;
	char buf[UNICODE_CHAR_LENGTH] = {0x0, 0x0, 0x0, 0x0};
	char tmp[INT_TYPE_SIZE];
	unsigned int  value = 0;
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
			strncpy(buf, current, UNICODE_CHAR_LENGTH);
			current += UNICODE_CHAR_LENGTH;
			enc_->test_char((unsigned char*)&buf);
			string_type a_char(buf, enc_->howmanybytes());
			string_array aca(ca);
			aca.push_back(a_char);
			// debug
//			if (word->chars() == "\347\272\246" && a_char == "\346\227\246")
//				cerr << "I got you" << endl;

			memcpy(tmp, current, INT_TYPE_SIZE);

			/*
			 * the last byte is used for indicating where is the stop of a word
			 */
			int word_where = (int)tmp[3];
			tmp[INT_TYPE_SIZE - 1] = 0;

			value = bytes_to_int32(tmp);
			current += INT_TYPE_SIZE;

			/// save them in the array
			assert(value > 0);
			word_ptr_type ret_word = freq_.add(aca, enc_->lang(), value); //->address(count);

			if (word_where > 0) {
				string lparent = ret_word->subchars(0, word_where);
				string rparent = ret_word->subchars(word_where, ret_word->size() - word_where);

				word_ptr_type a_word = freq_.find(lparent);
				assert(a_word != NULL);
				ret_word->lparent(a_word);
				a_word->is_word(true);

				a_word = freq_.find(rparent);
				assert(a_word != NULL);
				ret_word->rparent(a_word);
				a_word->is_word(true);
			}
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

			strncpy(buf, current, UNICODE_CHAR_LENGTH);
			current += UNICODE_CHAR_LENGTH;
			memcpy(tmp, current, INT_TYPE_SIZE);
			value = bytes_to_int32(tmp);
			assert(value > 0);
			current += INT_TYPE_SIZE;

			/// save them in the array
			enc_->test_char((unsigned char*)&buf);
			string_type a_char(buf, enc_->howmanybytes());
			string_array aca = ca;
			aca.insert(aca.begin(), a_char);
			word_ptr_type ret_word = freq_.add(aca, enc_->lang(), value); //->address(count);
			//cerr << "add new word: " << ret_word->chars() << " " << ret_word->freq() << endl;
			count++;
		}
	}
	word->set_loaded(true);
}

void FreqFile::load_index()
{
	if (wlen_ > 1) {
		idxf_.path(path_);
		idxf_.wlen(wlen_);
		idxf_.read(freq_);
	}
}

void FreqFile::load()
{
	array_type& word_array = freq_.array_k(wlen_ - 1);
	for (int i = 0; i < word_array.size(); ++i)
		read_term(word_array[i]);
}
