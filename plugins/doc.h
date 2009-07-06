/*
 * doc.h
 *
 *  Created on: Nov 27, 2008
 *      Author: monfee
 */

#ifndef DOC_H_
#define DOC_H_

//#include <string>
#include <stpl/stpl_unichar.h>
#include <stpl/stpl_unigrammar.h>
#include <stpl/stpl_parser.h>
#include "word.h"

class Doc {
public:
	typedef Word::string_type							string_type;
	typedef stpl::UNICODE::UniChar<stpl::CHINESE + stpl::NUMBER + stpl::PUNCTUATION, string_type>		chinese_char;
	typedef stpl::UNICODE::UniChar<stpl::ENGLISH + stpl::NUMBER + stpl::PUNCTUATION, string_type>		english_char;
	typedef stpl::UNICODE::UniChar<stpl::UNKNOWN, string_type>		uni_char;
	typedef stpl::Parser<stpl::GeneralGrammar<stpl::Document<stpl::StringBound<string_type> > > >
																	parser_type;
	typedef parser_type::document_type								document_type;
	typedef document_type::entity_iterator							entity_iterator;
	typedef document_type::entity_type								entity_type;

	typedef stpl::Parser<stpl::UNICODE::UniCharGrammar<uni_char> >
																	chinese_parser;
	typedef stpl::Parser<stpl::UNICODE::UniCharGrammar<english_char> >
																	english_parser;
	typedef stpl::Parser<stpl::UNICODE::UniCharGrammar<uni_char> >
																	unicode_parser;

private:
	stpl::Language	lang_;
	unsigned int 	ilang_;
	string_type& 	stream_;
	parser_type* 	parser_ptr_;
	entity_iterator pos_;

	bool			skip_acronym_;

	std::string 			name_;

public:
	Doc(stpl::Language lang, string_type& stream);
	Doc(std::string name, stpl::Language lang, string_type& stream);
	//Doc(stpl::Language lang);
	Doc(unsigned int ilang, string_type& stream);
	//Doc();
	~Doc();

	void parse();

	document_type& doc() { return parser_ptr_->doc(); }
	bool more();
	void next(entity_iterator& begin, entity_iterator& end);
	void reset();

	void skip_acronym(bool b = true) { skip_acronym_ = b; }
	bool do_skip_acronym() { return skip_acronym_; }

	const std::string& name() const { return name_; }
	void name(std::string name) { name_ = name; }

private:
	void init();

	bool skip();
};

#endif /* DOC_H_ */
