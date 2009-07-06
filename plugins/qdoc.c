/*
 * qdoc.cpp
 *
 *  Created on: Mar 12, 2009
 *      Author: monfee
 */

#include "qdoc.h"
#include "uniseg_settings.h"
#include "convert.h"
#include "qzer.h"
#include "qfreq.h"

QDoc::QDoc(Doc* doc, std::set<string_type>* seged_ptr)
	: doc_(doc)
	, File::File()
	, seged_ptr_(seged_ptr){

	init();
}

QDoc::~QDoc(){
	if (iofs_.is_open())
		iofs_.close();

}

void QDoc::init() {

	doc_->reset();
	while (doc_->more()) {
		entity_iterator begin;
		entity_iterator end;

		doc_->next(begin, end);

		sents_.push_back(string_type());
		string_type& sent = sents_[sents_.size() - 1];

		while (begin <= end) {
			string_type str = (*begin)->to_string();
			if (UNISEQ_settings::instance().lang() == stpl::ENGLISH) {
				tolower(str);
			}
			//sents_[sents_.size() - 1].append(str);
			sent.append(str);
			begin++;
		}
		cout << sent << endl;

	}

	if (UNISEQ_settings::instance().do_save()) {
		this->EXT_NAME = "txt";
		name(doc_->name());
		path(UNISEQ_settings::instance().opath() + string(UNISEQ_settings::instance().separator()));
		wopen();
	}
}

void QDoc::qzerit() {
	doc_->reset();
	while (doc_->more()) {
		entity_iterator begin;
		entity_iterator end;
		entity_iterator last;

		doc_->next(begin, end);

		//string_type	last_word("");

		do {
			string_type str("");
			string_type a_char("");
			int count = 0;

			while (begin <= end) {

				if (/*(str.length() > 0
						&& (!QFreq::instance().freq().find(str))
						&& (QFreq::instance().freq().array_size() >= UNISEQ_settings::instance().max())
						)
					|| */count >= UNISEQ_settings::instance().max()) {
					break;
				}

				last = begin;

				a_char = (*begin)->to_string();
				if (UNISEQ_settings::instance().lang() == stpl::ENGLISH) {
					tolower(a_char);
				}
				//sents_[sents_.size() - 1].append(a_char);
				str.append(a_char);
				begin++;
				count++;
			}

			//str.insert(0, last_word);

			QZer qzer;
			if (UNISEQ_settings::instance().verbose())
				cout << "Now segmenting:" << str << endl;

//			if (str == "楊椗光")
//				cout << "I caught you" << endl;

			qzer.doit(str);

			if (qzer.list().size() > 1
					&& last < end
					/*&& qzer.list().back()->size() == 1*/) {
				//begin--;
				//last_word = qzer.list().back()->chars();

//				int count_sofar = 0;
//				for (int i = 0; i < qzer.list().size(); i++) {
//					int tmp_size =qzer.list().front()->size();
//					if ( tmp_size <= 3)
//						count_sofar += tmp_size;
//
//				}

				begin -= qzer.list().back()->size();
				qzer.list().pop_back();
			}

			/**
			 * if the segment size is greater than three, then check whether it can be further broken down
			 */
			if (UNISEQ_settings::instance().repeat()) {
				array_type::iterator it = qzer.list().begin();
				for (; it != qzer.list().end();) {
					if ((*it)->size() >= 3) {
						QZer ano_qzer;

						if (UNISEQ_settings::instance().verbose())
							cout << "Checking segment:" << (*it)->chars() << endl;

						ano_qzer.doit((*it)->chars());

						if (ano_qzer.list().size() > 1) {

							// check if containing stop words, assuming stop words won't happen in middle of the words
							word_ptr_type first_w = ano_qzer.list().front();
							word_ptr_type last_w = ano_qzer.list().back();
							bool break_flag = true;
							if (first_w->size() == 1
									|| last_w->size() == 1){

								// if stop words found, then further break down, no then no
								if (!(UNISEQ_settings::instance().stopwords().find(first_w->chars())
										|| UNISEQ_settings::instance().stopwords().find(last_w->chars())))
									break_flag = false;
							}

							if (break_flag) {
								if (UNISEQ_settings::instance().do_debug()) cout << "Removing " << (*it)->chars() << endl;
								it = qzer.list().erase(it);

								array_type::iterator insert_pos = it;

								if (UNISEQ_settings::instance().do_debug()) cout << "Now the current is " << ((insert_pos != qzer.list().end())? (*insert_pos)->chars() : "THE END" )<< endl;

								for (array_type::iterator iner_it = ano_qzer.list().begin();
										iner_it != ano_qzer.list().end();
										iner_it++) {
									if (UNISEQ_settings::instance().do_debug()) cout << "Insert before " << ((insert_pos != qzer.list().end())? (*insert_pos)->chars() : "THE END" ) << endl;
									insert_pos = qzer.list().insert(insert_pos, (*iner_it));
									insert_pos++;
								}
								it = insert_pos;
								if (UNISEQ_settings::instance().do_debug()) cout << "Confirm the current is " << ((insert_pos != qzer.list().end())? (*insert_pos)->chars() : "THE END" ) << endl;

								if (it == qzer.list().end()) {
									if (qzer.list().size() > 1 && last < end) {
										if (UNISEQ_settings::instance().do_debug()) cout << "popping back " << qzer.list().back()->chars() << endl;
										begin -= qzer.list().back()->size();
										qzer.list().pop_back();
										break;
									}
								}
								continue;
							}
						}
					}
					it++;
				}
			}

			qzer.show();
			collect(qzer.list());
			if (UNISEQ_settings::instance().do_save())
				save(qzer.list());

			if (UNISEQ_settings::instance().verbose())
				cout << "Finished segmentation of " << str << endl;

		} while (begin <= end);

		//for (int i = 0; i < sents_.size(); i++) {
		//	string_type& sent = sents_[i];
//		int pos = 0;
//		do {
//			string_type temp = sent.substr(pos, 1);
//			int i = 2;
//			for (; i <= (sent.length() - pos); i++) {
//				string_type substr = sent.substr(pos, i);
//				if (!QFreq::instance().freq().find(substr)) {
//					break;
//				}
//
//				temp = substr;
//			}
//			--i;
//			pos += i;
//
//		} while(pos < sent.length());
	}
}

void QDoc::save(const array_type& list) {
	if (iofs_.is_open()) {
		for (int i = 0; i < list.size(); i++)
			iofs_ << list[i]->chars() << " ";
	    iofs_ << endl;
	}
}

void QDoc::collect(const array_type& list) {
	for (int i = 0; i < list.size(); i++) {
    	string_type word = list[i]->chars();
    	if (!(seged_ptr_->find(word) != seged_ptr_->end()))
    		seged_ptr_->insert(word);
    }
}
