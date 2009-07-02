/*
 * doc.cpp
 *
 *  Created on: Nov 27, 2008
 *      Author: monfee
 */

#include "doc.h"
#include "qconf.h"

Doc::Doc(std::string name, stpl::Language lang, string_type& stream) :
	name_(name), lang_(lang), stream_(stream), parser_ptr_(NULL) {
	ilang_ = lang_;

	init();
}

Doc::Doc(stpl::Language lang, string_type& stream) :
	lang_(lang), stream_(stream), parser_ptr_(NULL) {
	ilang_ = lang_;

	init();
}

Doc::Doc(unsigned int ilang, string_type& stream) :
	lang_(stpl::UNKNOWN), ilang_(ilang), stream_(stream) {
	init();
}

void Doc::init() {
	skip_acronym_ = true;
}

/*Doc::Doc(stpl::Language lang) :
	lang_(lang), parser_ptr_(NULL), stream_("") {
	ilang_ = lang_;
}

Doc::Doc() : lang_(stpl::UNKNOWN), ilang_(lang_), stream_("") {

}*/

Doc::~Doc() {
	if (parser_ptr_ != NULL) {
		/*
		if (lang_ == stpl::CHINESE) {
			chinese_parser* temp_ptr = reinterpret_cast<chinese_parser*>(parser_ptr_);
			delete temp_ptr;
		} else if (lang_ == stpl::ENGLISH) {
			english_parser* temp_ptr = reinterpret_cast<english_parser*>(parser_ptr_);
			delete temp_ptr;
		} else
		*/
			delete parser_ptr_;
	}
}

void Doc::parse() {

	switch(lang_) {
	case stpl::CHINESE:
		parser_ptr_ =
			reinterpret_cast<parser_type*>(new chinese_parser(stream_.begin(), stream_.end()));
		break;
	case stpl::ENGLISH:
		parser_ptr_ =
			reinterpret_cast<parser_type*>(new english_parser(stream_.begin(), stream_.end()));
		break;
	case stpl::UNKNOWN:
		parser_ptr_ =
			reinterpret_cast<parser_type*>(new unicode_parser(stream_.begin(), stream_.end()));
		break;
	default:
		const unsigned int ilang = ilang_;
		parser_ptr_ =
			reinterpret_cast<parser_type*>(
						new stpl::Parser<stpl::UNICODE::UniCharGrammar<
												stpl::UNICODE::UniChar<
											stpl::CHINESE + stpl::ENGLISH + stpl::NUMBER
											, string_type
																	  >
																  >
									>(	stream_.begin(), stream_.end() )
					);
		break;
	}

	parser_ptr_->parse();
	reset();
}

void Doc::reset() {
	pos_ = parser_ptr_->doc().iter_begin();
}

bool Doc::more() {
	while (pos_ != doc().iter_end()) {
		if (!skip())
			if ((*pos_)->lang() == lang_)
				return true;
		pos_++;
	}
	return false;
}

void Doc::next(entity_iterator& begin, entity_iterator& end) {
	begin = pos_;

	stpl::Language lang = lang_;

	if ((*pos_)->lang() == stpl::NUMBER)
		lang = stpl::NUMBER;

	pos_++;
	entity_iterator stop = pos_;

	while (pos_ != doc().iter_end()) {
		if ((*pos_)->lang() != lang /*QConf::instance()->lang()*/)
			break;

		if (do_skip_acronym())
			if (skip())
				break;

		stop = ++pos_;
	}
	end = stop;

	//if (end == doc().iter_end())
		end--;
}

bool Doc::skip( ) {

	bool ret = false;

	if ((*pos_)->lang() == stpl::ENGLISH) {
		string_type str = (*pos_)->to_string();
		assert(str.length() > 0);

		entity_iterator next = pos_;
		next++;
		if (isupper(str[0]) && next != doc().iter_end()) {
			if ((*next)->to_string() == ".") {
				pos_ = next;
				ret = true;
			}
		}
	}

	return ret;
}
