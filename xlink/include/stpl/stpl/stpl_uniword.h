/******************************************************************************
 * This file is part of the Simple Text Processing Library(STPL).
 * 
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
 * @created  			31/Aug/2008
 * @author				Ling-Xiang(Eric) Tang 
 *
 *******************************************************************************/

#ifndef STPL_UNIWORD_H_
#define STPL_UNIWORD_H_

#include "stpl_word.h"
#include "lang/stpl_unicode.h"
#include "lang/stpl_chinese.h"

namespace stpl {

	namespace UNICODE {
		/**
		 * Utf8Word is a word/phrase that is separated by the symbols and punctuation
		 * If the word consistpl of ascii characters only, then it is separated by whitespace as well
		 */
		template <typename StringT = std::string
				, typename IteratorT = typename StringT::iterator
				 >
		class Utf8Word: public Word<StringT, IteratorT> {
			public:
				typedef Utf8Word	entity_type;
				typedef	StringT		string_type;
				typedef IteratorT	iterator;		
							
				
			public:
				Utf8Word() : Word<StringT, IteratorT>::Word(){  }
				Utf8Word(IteratorT it) : Word<StringT, IteratorT>::Word(it) { 
				}
				Utf8Word(IteratorT begin, IteratorT end) :
					Word<StringT, IteratorT>::Word(begin, end) {
				}
				Utf8Word(StringT content) {
					Word<StringT, IteratorT>::Word(content);
				}
				virtual ~Utf8Word() {}	
				
				/**
				 * TODO
				 * decide the character whether it is symbol or punctuation
				 */
				static bool is_symbol_punct(IteratorT it) { 
					return false;
				}
				
			protected:	
				virtual bool is_valid_char(IteratorT it) {
					return (UNICODE::Utf8<StringT, IteratorT>::is_valid_char(it) > 0)
								&& !is_symbol_punct(it);			
				}			
		};	
	
		
		/**
		 * A Chinese word/phrase is separated by non chinese characters including symbols,
		 * punctuation, and characters in other langauges. It is unsegmented.
		 */
		template <typename StringT = std::string
				, typename IteratorT = typename StringT::iterator
				 >	
		class ChineseWord: public Utf8Word<StringT, IteratorT> {
			public:
				typedef ChineseWord	entity_type;
				typedef	StringT		string_type;
				typedef IteratorT	iterator;		
							
				
			public:
				ChineseWord() : Utf8Word<StringT, IteratorT>::Utf8Word(){  }
				ChineseWord(IteratorT it) : Utf8Word<StringT, IteratorT>::Utf8Word(it) { 
				}
				ChineseWord(IteratorT begin, IteratorT end) :
					Utf8Word<StringT, IteratorT>::Utf8Word(begin, end) {
				}
				ChineseWord(StringT content) {
					Utf8Word<StringT, IteratorT>::Utf8Word(content);
				}
				virtual ~ChineseWord() {}		
				
			protected:	
				virtual bool is_valid_char(IteratorT it) {
					return Chinese<StringT, IteratorT>::is_valid_char(it);			
				}			
		};		
	}
}

#endif /*STPL_UNIWORD_H_*/
