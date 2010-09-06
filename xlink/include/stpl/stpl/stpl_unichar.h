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
 * @created  			21/Sep/2008
 * @author				Ling-Xiang(Eric) Tang
 *
 *******************************************************************************/

#ifndef STPL_UNICHAR_H_
#define STPL_UNICHAR_H_

#include "stpl_entity.h"
#include "lang/stpl_unicode.h"
#include "lang/stpl_chinese.h"

namespace stpl {

	namespace UNICODE {

		/**
		 * Chinese Char is a single Chinese character
		 */
		template <typename StringT = std::string
				, typename IteratorT = typename StringT::iterator
				 >
		class ChineseChar: public StringBound<StringT, IteratorT> {
			public:
				typedef ChineseChar	entity_type;
				typedef	StringT		string_type;
				typedef IteratorT	iterator;


			public:
				ChineseChar() : StringBound<StringT, IteratorT>::StringBound(){  }
				ChineseChar(IteratorT it) : StringBound<StringT, IteratorT>::StringBound(it) {
				}
				ChineseChar(IteratorT begin, IteratorT end) :
					StringBound<StringT, IteratorT>::StringBound(begin, end) {
				}
				ChineseChar(StringT content) {
					StringBound<StringT, IteratorT>::StringBound(content);
				}
				virtual ~ChineseChar() {}

				virtual bool is_valid_char(IteratorT it) {
					return Chinese<StringT, IteratorT>::is_valid_char(it);
				}

				virtual Language lang() { return CHINESE; }

			protected:

				/**
				 * if the character is a chinese one, then which means a immediate end
				 */
				virtual bool is_end(IteratorT& it) {
					it += UNICODE::Utf8<StringT, IteratorT>::bytes(it);
					return true;
				}
		};

		/**
		 * Chinese Char is a single Chinese character
		 */
		template < unsigned int LANG = UNASSIGNED
				, typename StringT = std::string
				, typename IteratorT = typename StringT::iterator
				 >
		class UniChar: public StringBound<StringT, IteratorT> {
			public:
				typedef UniChar	entity_type;
				typedef	StringT		string_type;
				typedef IteratorT	iterator;

			private:
				Language lang_;

			public:
				UniChar(IteratorT begin, IteratorT end) :
					StringBound<StringT, IteratorT>::StringBound(begin, end) {
					init();
				}
				UniChar() : StringBound<StringT, IteratorT>::StringBound(){ init(); }
				//UniChar(Language lang) : lang_(lang), StringBound<StringT, IteratorT>::StringBound(){  }

				virtual ~UniChar() {}

				virtual bool is_valid_char(IteratorT it) {

					lang_ = UNKNOWN;
					Language lang = static_cast<Language>(LANG & UNKNOWN);

					if ((LANG & CHINESE) == CHINESE || lang == UNKNOWN) {
						if (Chinese<StringT, IteratorT>::is_valid_char(it)) {
							lang_ = CHINESE;

							return true;
						}
					}

					if ((LANG & ENGLISH) == ENGLISH || lang == UNKNOWN) {
						if (isalpha(*it)) {
							lang_ = ENGLISH;

							return true;
						}
					}

					if ((LANG & NUMBER) == NUMBER || lang == UNKNOWN) {
						if (isdigit(*it)) {
							lang_ = NUMBER;

							return true;
						}
					}

					if ((LANG & SPACE) == SPACE || lang == UNKNOWN) {
						if (isspace(*it)) {
							lang_ = SPACE;

							return true;
						}
					}

					if ((LANG & PUNCTUATION) == PUNCTUATION || lang == UNKNOWN) {
						if (ispunct(*it)) {
							lang_ = PUNCTUATION;

							return true;
						}
					}

					if ((LANG & SYMBOL) == SYMBOL || lang == UNKNOWN) {
						if (ispunct(*it)) {
							lang_ = SYMBOL;

							return true;
						}
					}

					return (lang == UNKNOWN);
				}

				virtual Language lang() {
					//return (LANG == UNASSIGNED) ? LANG : lang_;
					return lang_;
				}

			protected:

				/**
				 * if the character is a chinese one, then which means a immediate end
				 */
				virtual bool is_end(IteratorT& it) {
					it += UNICODE::Utf8<StringT, IteratorT>::bytes(it);
					return true;
				}

			private:
				void init() {
					lang_ = UNASSIGNED;
				}
		};
	}
}

#endif /*STPL_UNICHAR_H_*/
