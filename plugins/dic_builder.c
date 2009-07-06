/*
 * dic_builder.cpp
 *
 *  Created on: Feb 12, 2009
 *      Author: monfee
 */

#include "dic_builder.h"
#include "doc.h"
#include "uniseg_settings.h"
#include "convert.h"
#include <fstream>

using namespace std;

DicBuilder::DicBuilder(Dic* dic) : dic_(dic) {
	count_ = 0;
}
void DicBuilder::add(string_type word, stpl::Language lang) {
	dic_->add(word, lang);
	count_++;
}

void DicBuilder::build(string_type& text, stpl::Language lang) {

	typedef Doc::entity_iterator	iterator;
	Doc doc(UNISEQ_settings::instance().lang(), text);

	doc.parse();
	doc.reset();
	while (doc.more()) {

		iterator begin;
		iterator end;

		doc.next(begin, end);

		string_type word;
		while (begin <= end) {
			iterator next = begin;

			string_type str = (*begin)->to_string();
			if (UNISEQ_settings::instance().lang() == stpl::ENGLISH) {
				tolower(str);
			}
			word.append( str );

			if (next != end) {
				++next;
				if ((*begin)->end() != (*next)->begin()) {
					add(word, lang);
					word.clear();
				}
					//cout << " ";
			} else {
				//cout << endl;
				add(word, lang);
				break;
			}
			begin = next;
		}

	}
	//cout << endl;
}

