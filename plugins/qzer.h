#ifndef QZER_H_
#define QZER_H_

#include "freq.h"
#include <vector>
#include "uniseg_types.h"
#include "seg.h"

class QZer {

private:
	Freq& 						freq_;
	std::vector<int> 			stat_;
	int 						k_;
	//Freq cwlist_;
	array_type					cwlist_;


public:
	QZer();
	~QZer();

	void start();
	void show();
	void save();

	const array_type& list() const { return cwlist_; }
	array_type& list() { return cwlist_; }
	void doit(const word_ptr_type w_ptr);
	void doit(const string_type& phrase);

private:
	void init();
	void do_others(Seger& seger);
};

#endif // QZER_H_
