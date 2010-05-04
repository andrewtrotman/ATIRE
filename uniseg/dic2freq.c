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
