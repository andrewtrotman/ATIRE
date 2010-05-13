#include "freq_loader.h"
#include "freq_file.h"
#include "convert.h"
#include "uniseg_settings.h"

#include <iostream>

using namespace std;

void FreqLoader::load(std::string path, int n) {
	k_ = 1;
	//	int num = 0;
	cerr << "Loading files from " << path << endl;
	UNISEG_settings::instance().load = true;

	while (k_ <= n) {


		FreqFile freq_file(stringify(k_), &freq_);
		freq_file.path(path);
		freq_file.wlen(k_);

		if (!freq_file.exist()) {
			k_--;
			std::cerr << "No such file:" << freq_file.fullpathname() << std::endl;
			break;
		}

		if (k_ > 1)
			freq_file.read_with_index();
		else
			freq_file.read();

		k_++;
	}
}

void FreqLoader::save(std::string path) {

	int k = 1;

	while(1) {

		cerr << "saving freq for words with size " << k << endl;

		if (freq_.array_k_size(k) <= 0) {
			cerr << "No more words to save for size " << k << endl;
			break;
		}

		FreqFile freq_file(stringify(k), &freq_);
		//freq_file.path(UNISEG_settings::instance().wd());
		freq_file.path(path);
		freq_file.wlen(k);
		freq_file.write(freq_.array_k(k));

		if (k > 1) {
			IndexFile idx_file(stringify(k));
			//idx_file.path(UNISEG_settings::instance().wd());
			idx_file.path(path);
			idx_file.wlen(k);
			idx_file.alloc(freq_.array_k(k));
			idx_file.write();
		}

		k++;
	}

	k_ = k;
}
