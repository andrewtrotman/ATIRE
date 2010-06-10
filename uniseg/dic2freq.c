/*
 * dic2freq.cpp
 *
 *  Created on: May 4, 2010
 *      Author: monfee
 */

#include "dic2freq.h"
#include "dic.h"
#include "freq.h"
#include "uniseg_string.h"

#include <iostream>

using namespace std;

Dic2Freq::Dic2Freq()
{

}

Dic2Freq::~Dic2Freq()
{
}

void Dic2Freq::dic2freq(Dic *dic, Freq *freq, long lang)
{
	const Dic::word_map& list = dic->list(); //dic_.list();
	Dic::word_map::const_iterator iter;
	for (iter=list.begin(); iter != list.end(); ++iter) {
		string_array ca;
		to_string_array(iter->first, ca);
		freq->add(ca, lang, iter->second);
	}

}

void Dic2Freq::dic2freq2(Dic *dic, Freq *freq, long lang)
{
	const Dic::word_map& list = dic->list(); //dic_.list();
	Dic::word_map::const_iterator iter;
	word_ptr_type dic_word = NULL;
	for (iter=list.begin(); iter != list.end(); ++iter) {
		dic_word = freq->find(iter->first);
		if (!dic_word) {
			string_array ca;
			to_string_array(iter->first, ca);
			dic_word = freq->add(ca, lang, 1);
			dic_word->adjust(1, true);
		}
		else {
			if (dic_word->freq() == 0)
				dic_word->freq(1);
		}
		dic_word->is_word(true);
	}

	dic_word = freq->find("密林");
	cerr << "stop here" << endl;

}
