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
	typedef std::map<string_type, uniseg_encoding::language>	word_map;
	typedef std::map<int, int>						stat_map;

private:
	word_map										list_;
	stat_map										stat_;

	//uniseg_encoding::language									lang_;

public:
	Dic() {}
	~Dic() {}

	const word_map& list() const { return list_; }
	const stat_map& stat() const { return stat_; }

	void add(string_type, uniseg_encoding::language);

	bool find(string_type word);
	//uniseg_encoding::language lang() { return lang_; }
	void save(string_type filename, uniseg_encoding::language lang);
	void load(string_type filename, uniseg_encoding::language lang);

	int size() { return list_.size(); }

	void update_stat(int num, int howmany);
	int find_stat(int num);

	void show();
};

#endif /* DIC_H_ */
