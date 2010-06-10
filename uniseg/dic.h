/*
 * dic.h
 *
 *  Created on: Feb 14, 2009
 *      Author: monfee
 */

#ifndef DIC_H_
#define DIC_H_

#include "uniseg_types.h"
#include "address.h"
#include <map>

class Dic {
public:
	typedef std::map<string_type, long>				word_map;
	typedef std::map<int, int>						stat_map;

private:
	word_map										list_;
	stat_map										stat_;

	long											lang_;

public:
	Dic(long lang = 0) : lang_(lang) {}
	~Dic() {}

	const word_map& list() const { return list_; }
	const stat_map& stat() const { return stat_; }

	long lang() { return lang_; }
	void lang(long l) { lang_ += l; }

	void add(string_type, long freq);

	bool find(string_type word);
	bool fuzzy_search(const string_type word);
	bool fuzzy_search_right(const string_type word);
	//long lang() { return lang_; }
	void save(string_type filename);
	void load(string_type filename, long lang);
	void load(string_type filename);

	int size() { return list_.size(); }

	void update_stat(int num, int howmany);
	int find_stat(int num);

	void show();
};

#endif /* DIC_H_ */
