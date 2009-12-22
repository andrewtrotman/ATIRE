#ifndef CLIST_H_
#define CLIST_H_

#include "cwords.h"

typedef std::list<CWords*> 			cwords_list;
typedef cwords_list::iterator		cwords_list_iterator;

class CList {
private:
	cwords_list 				list_;

public:
	CList();
	CList(int n, CWords* cwords); // make n copies of it
	~CList();

	const cwords_list& list() const { return list_; }
	cwords_list& list() { return list_; }

	void clear_node(cwords_list_iterator& it);

	bool initialize();
	void append(CWords* cwords);
	void append(const CList& clist);
	void insert(CWords* cwords);
	void insert(const CList& clist);

	void show();
	void cal(Freq* freq);
	void sort(bool desc = true);
	static bool cmp_score(CWords *cw1, CWords *cw2);
	static bool cmp_score_ascending(CWords *cw1, CWords *cw2);

	void remove_no_end();

	// apply rules
	void remove(int freq, word_ptr_type tw_ptr = NULL);
	void remove_two_consecutive_single_chars();

	CWords* front();
	CWords* second();
	int size() { return list_.size(); }

	cwords_list_iterator delete_node(cwords_list_iterator it);
	void apply_rules();
};

#endif /*CLIST_H_ */
