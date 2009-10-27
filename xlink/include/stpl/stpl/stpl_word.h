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
 * @author				Ling-Xiang(Eric) Tang 
 *
 *******************************************************************************/

#ifndef STPL_WORD_H_
#define STPL_WORD_H_

#include "stpl_entity.h"
#include "characters/stpl_character.h"
#include "characters/stpl_chinese.h"

namespace stpl {
	
	/**
	 *  a word is the characters set usually seperated by whitespace, and 
	 *  all sorts of symbols
	 */
	template <typename StringT = std::string
			, typename IteratorT = typename StringT::iterator
			 >
	class Word : public StringBound<StringT, IteratorT> {
		public:
			typedef Word	entity_type;
			typedef	StringT	string_type;
			typedef IteratorT	iterator;		
						
		public:
			static const StringT DEFAULT_SEPARATOR;
			
		private:
			StringT	separator_;
			StringT special_;   // contains the special characters that is part of the word,
								// including symbol, punctuation 
			
		private:
			void init() {
				separator_ = "";
			}
			
		public:
			Word() : StringBound<StringT, IteratorT>::StringBound(){ init(); }
			Word(IteratorT it) : StringBound<StringT, IteratorT>::StringBound(it) { 
				init();
			}
			Word(IteratorT begin, IteratorT end) :
				StringBound<StringT, IteratorT>::StringBound(begin, end) {
				init(); 
			}
			Word(StringT content) {
				StringBound<StringT, IteratorT>::StringBound(content);
				init();
			}
			virtual ~Word() {}	
	
			void content(StringT content) {
				 begin(content.begin());
				 end(content.end());
			}
			
			void separator(StringT separator) {
				separator_ = separator;
			}
			
			void set_default() {
				separator_ = DEFAULT_SEPARATOR;
			}
			
			virtual bool is_keyword() { return false; }
			static bool is_key() { return false; }
			
			/**
			 * Actually there is no relationship between valid chars and separators
			 * however, if separators are set then, that means word is separated by separators
			 * rather then judeged by what valid chars make word 
			 */
			virtual bool is_valid_char(IteratorT it) {
				if (separator_.length() > 0) {
					if (separator_.find(*it) == StringT::npos)
						return true;
					return false;
				}
				return isalnum(*it);			
			}			
			
		protected:
			virtual bool is_start(IteratorT& it) {
				bool ret = is_valid_char(it);
				if (ret)
					this->begin(it);
				return ret;
			}
			
			virtual bool is_end(IteratorT& it) {				
				return StringBound<StringT, IteratorT>::is_end(it) || !is_valid_char(it);
			}		
			
			
			virtual IteratorT skip_not_valid_char(IteratorT& it) {
				return skip_whitespace(it);
				//return skip_non_alnum_char(it);
				//return it;
			}
					
	};
	
	template < typename StringT
			, typename IteratorT
			 >
	const StringT Word<StringT, IteratorT>::DEFAULT_SEPARATOR(" \n\t\r");
	
	
	template <typename StringT = std::string
			, typename IteratorT = typename StringT::iterator
			 >
	class AlnumWord: public Word<StringT, IteratorT> {
		public:
			typedef AlnumWord	entity_type;
			typedef	StringT	string_type;
			typedef IteratorT	iterator;		
						
			
		public:
			AlnumWord() : Word<StringT, IteratorT>::Word(){  }
			AlnumWord(IteratorT it) : Word<StringT, IteratorT>::Word(it) { 
			}
			AlnumWord(IteratorT begin, IteratorT end) :
				Word<StringT, IteratorT>::Word(begin, end) {
			}
			AlnumWord(StringT content) {
				Word<StringT, IteratorT>::Word(content);
			}
			virtual ~AlnumWord() {}		
			
			virtual bool is_valid_char(IteratorT it) {
				return isalnum(*it);			
			}			
			
			virtual Language lang() { return ENGLISH; }
	};
}
#endif /*STPL_WORD_H_*/
