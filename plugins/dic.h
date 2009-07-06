/*
 * dic.h
 *
 *  Created on: Feb 14, 2009
 *      Author: monfee
 */

#ifndef DIC_H_
#define DIC_H_

#include <stpl/characters/stpl_character.h>
#include "uniseg_types.h"
#include <map>

class Dic {
public:
	typedef std::map<string_type, stpl::Language>	word_map;
	typedef std::map<int, int>						stat_map;

private:
	word_map										list_;
	stat_map										stat_;

	//stpl::Language									lang_;

public:
	Dic() {}
	~Dic() {}

	const word_map& list() const { return list_; }
	const stat_map& stat() const { return stat_; }

	void add(string_type, stpl::Language);

	bool find(string_type word);
	//stpl::Language lang() { return lang_; }
	void save(string_type filename, stpl::Language lang);
	void load(string_type filename, stpl::Language lang);

	int size() { return list_.size(); }

	void update_stat(int num, int howmany);
	int find_stat(int num);

	void show();
};

#endif /* DIC_H_ */
