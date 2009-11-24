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

	for (unsigned int i = 0; i < arr.size(); i++) {

		char *chars;
		word_ptr_type word_ptr = arr[i];
		unsigned int freq = 0;

		chars = (char*)(word_ptr->family().second->chars().c_str());

		int len = strlen(chars);
		assert(len <= UNICODE_CHAR_LENGTH);

		freq = word_ptr->freq();

		iofs_.write(chars, UNICODE_CHAR_LENGTH);
		iofs_.write((char *)&freq, sizeof(unsigned int));
	}


	ofstream::pos_type pos = iofs_.tellp();
	cerr << "number of word: " << arr.size() << endl;
	//cerr << "expected file size: " << size << endl;
	cerr << "actual file size: " << static_cast<int>(pos) << endl;
	//assert(size == static_cast<int>(pos));

	iofs_.close();
}

void FreqFile::read() {

	cerr << "loading " << name_ << endl;

	char buf[UNICODE_CHAR_LENGTH] = {0x0, 0x0, 0x0, 0x0};
	char tmp[INT_TYPE_SIZE];
	unsigned int  value;

	assert(wlen_ == 1);

	File::ropen();
	File::read();
	char *current = buf_;
	uniseg_encoding *enc = uniseg_encoding_factory::instance().get_encoding();

	if (!iofs_) {
		// An error occurred!
		// iofs_.gcount() returns the number of bytes read.
		// calling iofs_.clear() will reset the stream state
		// so it is usable again.
		cerr << "unable to open file \"" << name_ << "\"" << endl;
	} else {
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
			int ret = sscanf(tmp, "%i", &value);
			assert(ret == INT_TYPE_SIZE);
			current += INT_TYPE_SIZE;
			string_array ca;
			/// save them in the array
			enc->test_char((unsigned char*)&buf);
			string_type a_char(buf, enc->howmanybytes());
			ca.push_back(a_char);

			freq_.add(ca, enc->lang(), value); //->address(count/RECORD_LENGTH);
			count += RECORD_LENGTH;
			// calculating the total characters appear in the corpus
			//if (k == 1)
			//	num += value;
		}

		cerr << "loaded number of word with size(" << wlen_ << ") : " << freq_.array_k_size(wlen_) << endl;
		//if (k == 1)
		//	cerr << "total characters: " << num << " approximily " << num*3/(1024*1024) << "m" << endl;
	}
	iofs_.close();
}

void FreqFile::read_with_index() {

	cerr << "loading " << name_ << endl;

	char buf[UNICODE_CHAR_LENGTH] = {0x0, 0x0, 0x0, 0x0};
	unsigned int  value = 0;

	uniseg_encoding *enc = uniseg_encoding_factory::instance().get_encoding();

	IndexFile idxf(name_);
	idxf.path(path_);
	idxf.wlen(wlen_);
	idxf.read();
	File::ropen();

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
				iofs_.seekg(pos, ios::beg);
				assert(pos < size_);

				for (unsigned int j = llb; j <= lhb && j != Address::INVALID_BOUND; j++) {
					if (!iofs_.read (buf, UNICODE_CHAR_LENGTH)
							|| !iofs_.read ((char *)&value, sizeof(unsigned int))) {
						// Same effect as above
						cerr << "Errors when reading file \"" << filename_
							<< "\" with address(" << j << ")" << endl;
						continue;
					}

					/// save them in the array
					enc->test_char((unsigned char*)&buf);
					string_type a_char(buf, enc->howmanybytes());
					string_array aca(ca);
					aca.push_back(a_char);
					assert(value > 0);
					freq_.add(aca, enc->lang(), value); //->address(count);
					count++;
				}
			}

			if (rlb != Address::INVALID_BOUND) {
				pos = rlb * RECORD_LENGTH;
				iofs_.seekg(pos, ios::beg);
				assert(pos < size_);

				for (unsigned int j = rlb; j <= rhb && j != Address::INVALID_BOUND; j++) {

					if (!iofs_.read (buf, UNICODE_CHAR_LENGTH)
							|| !iofs_.read ((char *)&value, sizeof(unsigned int))) {
						// Same effect as above
						cerr << "Errors when reading file \"" << filename_
							<< "\" with address(" << j << ")" << endl;
						continue;
					}

					/// save them in the array
					enc->test_char((unsigned char*)&buf);
					string_type a_char(buf, enc->howmanybytes());
					string_array aca = ca;
					aca.insert(aca.begin(), a_char);
					assert(value > 0);
					freq_.add(aca, enc->lang(), value); //->address(count);
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
}
