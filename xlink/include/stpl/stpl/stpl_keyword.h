/******************************************************************************
 * This file is part of the Simple Text Processing Library(STPL).
 * (c) Copyright 2007-2008 TYONLINE TECHNOLOGY PTY. LTD.
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU LESSER GENERAL PUBLIC LICENSE, Version 3 as published by the Free Software
 * Foundation and appearing in the file LICENSE.LGPL included in the
 * packaging of this file.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 *******************************************************************************
 *
 * @author				Ling-Xiang(Eric) Tang 
 *
 *******************************************************************************/

#ifndef STPL_KEYWORD_H_
#define STPL_KEYWORD_H_

#include "stpl_word.h"

namespace stpl {

	/**
	 *  Keyword is someting that is unchangable, and has special meaning 
	 */
	template <typename StringT = std::string, typename IteratorT = typename StringT::iterator>
	class Keyword : public Word<StringT, IteratorT> {
		public:		
			typedef	StringT	string_type;
			typedef IteratorT	iterator;

		public:
			Keyword() {	}
			Keyword(IteratorT it) : Word<StringT, IteratorT>::Word(it) {	}
			Keyword(IteratorT begin, IteratorT end) : Word<StringT, IteratorT>::Word(begin, end) {}
			Keyword(StringT content) {
				Word<StringT, IteratorT>::Word(content);
			}			
			virtual ~Keyword() {}
		/*
			Keyword() {}
			Keyword(StringT content) {
				Word<StringT, IteratorT>::Word(content);
			}
			virtual ~Keyword() {}	
			*/
			virtual bool is_keyword() { return true; }	
	};	
}
#endif /*STPL_KEYWORD_H_*/
