/*
 * freq_file.cpp
 *
 *  Created on: Oct 26, 2008
 *      Author: monfee
 */

#include "qconf.h"
#include "freq_file.h"

#include <string.h>
#include <stpl/characters/stpl_unicode.h>

#include <iostream>

using namespace stpl;
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

unsigned int FreqFile::cal(File::array_type& arr) {
	//if (arr[0]->address() == SEGMENTATION::INVALID)
	//	freq_.alloc(k);
	//File::word_ptr_type last_word = freq_n_[k - 1][freq_n_[k - 1].size() - 1];
	/*
	for (int j = 0; j < freq_n_[k - 1].size(); j++)
		cout << "last word: " << freq_n_[k - 1][j]->chars()
			<< " address: " << freq_n_[k - 1][j]->address()
			<< " freq: " << freq_n_[k - 1][j]->freq()<< endl;
			*/

	//t_size = last_word->address()
	//	+ last_word->freq() * 2 * 8/*(k -1)*/
	//	+ freq_n_[k - 1].size() * 8;
	return (UNICODE_CHAR_LENGTH + sizeof(int)) * arr.size();
}

void FreqFile::write(File::array_type& arr) {

	if (arr.size() <= 0)
		return;

	File::wopen();

	if(!iofs_) {
	    cout << "Cannot open file for writing.\n";
	    return;
	}

	cout << "expected total size: " << cal(arr) << endl;

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
	cout << "number of word: " << arr.size() << endl;
	//cout << "expected file size: " << size << endl;
	cout << "actual file size: " << static_cast<int>(pos) << endl;
	//assert(size == static_cast<int>(pos));

	iofs_.close();
}

void FreqFile::read() {

	cout << "loading " << name_ << endl;

	char buf[UNICODE_CHAR_LENGTH] = {0x0, 0x0, 0x0, 0x0};
	unsigned int  value;

	assert(wlen_ == 1);

	File::ropen();

	if (!iofs_) {
		// An error occurred!
		// iofs_.gcount() returns the number of bytes read.
		// calling iofs_.clear() will reset the stream state
		// so it is usable again.
		cerr << "unable to open file \"" << name_ << "\"" << endl;
	} else {
		int count = 0;

		while (count < size_) {
			if (!iofs_.read (buf, UNICODE_CHAR_LENGTH)
					|| !iofs_.read ((char *)&value, sizeof(unsigned int))) {
				// Same effect as above
				cerr << "Errors when reading file \"" << name_ << "\"" << endl;
			}
			string_array ca;
			/// save them in the array
			string_type a_char =
				UNICODE::Utf8<string_type, char*>::first_utf8_char((char*)&buf);
			ca.push_back(a_char);

			freq_.add(ca, value); //->address(count/RECORD_LENGTH);
			count += RECORD_LENGTH;
			// calculating the total characters appear in the corpus
			//if (k == 1)
			//	num += value;
		}

		cout << "loaded number of word with size(" << wlen_ << ") : " << freq_.array_k_size(wlen_) << endl;
		//if (k == 1)
		//	cout << "total characters: " << num << " approximily " << num*3/(1024*1024) << "m" << endl;
	}
	iofs_.close();
}

void FreqFile::read_with_index() {

	cout << "loading " << name_ << endl;

	char buf[UNICODE_CHAR_LENGTH] = {0x0, 0x0, 0x0, 0x0};
	unsigned int  value = 0;

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
					string_type a_char(
						UNICODE::Utf8<string_type, char*>::first_utf8_char((char*)&buf));
					string_array aca(ca);
					aca.push_back(a_char);
					assert(value > 0);
					freq_.add(aca, value); //->address(count);
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
					string_type a_char =
						UNICODE::Utf8<string_type, char*>::first_utf8_char((char*)&buf);
					string_array aca = ca;
					aca.insert(aca.begin(), a_char);
					assert(value > 0);
					freq_.add(aca, value); //->address(count);
					count++;
				}
			}
		}

		cout << "loaded number of word with size(" << wlen_ << ") : " << freq_.array_k_size(wlen_) << endl;
		cout << "actual number of word with size(" << wlen_ << ") : " << count << endl;
		//if (k == 1)
		//	cout << "total characters: " << num << " approximily " << num*3/(1024*1024) << "m" << endl;
	}

	iofs_.close();
}
