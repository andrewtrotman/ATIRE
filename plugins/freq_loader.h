#ifndef FREQ_LOADER_H_
#define FREQ_LOADER_H_

#include "freq.h"
#include <string>
#include <climits>

class FreqLoader {
private:
	Freq& 		freq_;
	int 		k_;


public:
	FreqLoader(Freq& freq) : freq_(freq) {  }
	~FreqLoader() {}

	void load(std::string path, int n = INT_MAX);
	void save(std::string path);

	const int count() const { return k_; }
};
#endif // FREQ_LOADER_H_
