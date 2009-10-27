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

#ifndef STPL_ATTRIBUTE_H_
#define STPL_ATTRIBUTE_H_

#include <string>
#include <algorithm>
#include <functional>
#include <iterator>

#include "stpl_entity.h"

namespace stpl {

	template <typename StringT = std::string, typename IteratorT = typename StringT::iterator>
	class Property : public StringBound<StringT, IteratorT> {
		protected:
			typedef StringBound<StringT, IteratorT> StringB;
			StringB name_;
			StringB	value_;
			bool has_equal_;
			bool has_quote_;
			bool force_end_quote_;  // if has quote, then it must end with quote
			bool is_single_quote_;  // false is double quote, true for single quote
			std::string end_chars_;

		private:
			void init() {
				has_equal_ = false;
				is_single_quote_ = false;
				has_quote_ = false;
				force_end_quote_ = false;
			}

		protected:
			virtual bool is_end_char(IteratorT& it) {
				if (end_chars_.length() > 0) {
					std::string::size_type pos = end_chars_.find(*it);
					if (pos != std::string::npos)
						return true;
				}
				else {
					if (has_equal_ && !has_quote_)
						return isspace(*it);
				}
				return false;
			}

			virtual bool is_start(IteratorT& it) {
				skip_whitespace(it);
				name_.begin(it);
				name_.end(it);
				value_.begin(it);
				value_.end(it);
				return StringBound<StringT, IteratorT>::is_start(it);
			}

			virtual bool is_end(IteratorT& it) {
				bool ret = eow(it);
				//	ret = (has_equal_ && (value_.begin() != value_.end()));

				if (!ret) {
					// if have the break character like "=, space" or something specified as the break char
					// which mean the beginning of VALUE
					if (has_equal_) {
						bool check_end_char = false;
						if (has_quote_) {
							IteratorT pre = it;
							pre--;
							bool ret1 = false;
							bool ret2 = false;
							if (( (ret1 = (*it == '\"'))
									|| (ret2 = (*it == '\''))
								  )
								 && *pre != '\\') {
								if (is_single_quote_)
									ret = ret2;
								else
									ret = ret1;

								if (ret)
									++it;
							}
							if (!ret && !force_end_quote_)
								check_end_char = true;
						} else {
							check_end_char = true;
						}
						if (check_end_char)
							ret = is_end_char(it);
					}
					else { // looking for the NAME part
						if (!(ret = is_end_char(it))) {

							name_.end(it);
							bool has_space = isspace(*it);

							if (has_space)
								skip_whitespace(it);

							if (*it == '=') {
								has_equal_ = true;

								// deside where is the end of name
								IteratorT pre = it;
								skip_whitespace_backward(--pre);
								name_.end(++pre);

								++it;
								skip_whitespace(it);
								if (*it == '\"' || *it == '\'') {
									is_single_quote_ = (*it == '\'');
									has_quote_ = true;
									++it;
								}
								value_.begin(it);
								value_.end(it);
								//--it;
							}

							if (has_space && !has_equal_)
								ret = true;
						}
					}
				} // (1) if (!ret)
				return ret;
			}

			virtual void add_name(StringT& name) {
				this->ref().append(name);
			}

			void add_equal_symbol() {
				this->ref().append("=");
			}

			virtual void add_value(StringT& value) {
				this->ref().append("\"");
				this->ref().append(value);
				this->ref().append("\"");
			}

			void end_notify(IteratorT& end) {
				if (has_equal_) {
					if (has_quote_) {
						IteratorT pre = end;
						pre--;
						skip_whitespace_backward(pre);
						if ( (*pre == '\"')	|| (*pre == '\'')) {
							  value_.end(pre);
						}
						else
							value_.end(pre++);
					}
					else
						value_.end(end);
				}
			}

		public:
			Property() :
				StringBound<StringT, IteratorT>::StringBound() { init(); }
			Property(IteratorT begin) :
				StringBound<StringT, IteratorT>::StringBound(begin) { init(); }
			Property(IteratorT begin, IteratorT end) :
				StringBound<StringT, IteratorT>::StringBound(begin, end) { init(); }
			Property (StringT content) :
				StringBound<StringT, IteratorT>::StringBound(content) {
				init();
			}
			virtual ~Property() {}

			StringT name() {
				return StringT(name_.begin(), name_.end());
			}

			StringT value() {
				StringT value(value_.begin(), value_.end());
				if (has_quote_) {
					//StringT::size_type pos;
					int pos = 0;
					while ((pos = value.find("\\\"")) != StringT::npos)
						value.replace(pos, 2, "\"");
				}
				return value;
			}

			void set_end_chars(std::string end_chars) {
				end_chars_ = end_chars;
			}

			void force_end_quote(bool b) { force_end_quote = b; }

			virtual bool match(IteratorT begin, IteratorT end) {
				if (StringBound<StringT, IteratorT>::match(begin, end)) {
					if (name_.length() <= 0)
						return false;
					return true;
				}
				return false;
			}

			virtual bool equal(StringT what) {
				return name_.to_string() ==  what;
			}

			virtual void create(StringT name, StringT value) {
				//add_name(name);
				//add_equal_symbol(text);
				//add_value(value);
				// TODO add the postion for name_, value_
				StringT  temp(name);

				temp.append("=");

				temp.append("\"");
				temp.append(value);
				temp.append("\"");

				this->ref().append(temp);
			}
	};
}
#endif /*STPL_ATTRIBUTE_H_*/
