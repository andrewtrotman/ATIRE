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

#ifndef STPL_ENTITY_H_
#define STPL_ENTITY_H_

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <climits>

#include "characters/stpl_character.h"

using namespace std;

namespace stpl {

	const char SPACING[] = {"  "};

	template <typename StringT = std::string, typename IteratorT = typename StringT::iterator>
	class StringBound : public Character<StringT, IteratorT> {
		public:
			typedef	StringT		string_type;
			typedef IteratorT	iterator;

		public:
			StringBound ()
				//:
				//begin_((IteratorT)content_.begin())
				//, end_((IteratorT)content_.end())
				{ init(); }
			StringBound (IteratorT it) :
				 begin_(it) , end_(it) { init(); }
			StringBound (IteratorT begin, IteratorT end) :
				 begin_(begin), end_(end) { init(); }
			StringBound (StringT content) /*: content_ref_(content_) */ {
				content_ = content;
				init();
				//begin(content_.begin());
				//end(content_.end());
			}
			//StringBound (StringT& content) :
			//	 content_ref_(content), begin_(content.begin()), end_(content.end()) {
			//}
			virtual ~StringBound() {}

			StringBound& operator= (StringBound& se) {
				if (this != &se) {
					begin_ = se.begin();
					end_ = se.end();
				}
				return *this;
			}

			void begin(IteratorT it) { begin_ = it; }
			const IteratorT begin() const { return begin_; }
			void end(IteratorT it) { end_ = it; }
			const IteratorT end() const { return end_; }

			virtual void bound(IteratorT begin, IteratorT end) {
				this->begin(begin);
				this->end(end);
			}

			size_t length() {
				//if (begin_ == NULL || end_ == NULL)
				//	return 0;
				 return end_ - begin_;
			}

			/**
			 * end of word or word-equivelant
			 */
			bool eow(IteratorT it) {
				//if (this->end())
				//if (begin_ == end_ )  // TODO find a better way to decide the end of the word
				//	return false;
				//else
					return it >= this->end();
				//return false;
			}

			IteratorT skip_n_chars(IteratorT& it, int n) {
				int count = 0;
				while (!this->eow(it) && count < n) {
					++it;
					++count;
				}
				return it;
			}

			IteratorT skip_whitespace(IteratorT& next) {
				//IteratorT next = it;
				while (!eow(next) && isspace(*next))
					next++;
				return next;
			}

			IteratorT skip_whitespace_backward(IteratorT& pre) {
				//IteratorT next = it;
				while (isspace(*pre))
					--pre;
				return pre;
			}

			IteratorT skip_non_alnum_char(IteratorT& next) {
				//IteratorT next = it;
				while (!eow(next) && !isalnum(*next))
					next++;
				return next;
			}

			virtual IteratorT skip_not_valid_char(IteratorT& next) {
				//return skip_whitespace(next);
				while (!eow(next) && !is_valid_char(next))
					next++;
				return next;
			}

			StringT to_string() {
				if (begin_ == end_)
					return StringT("");
				return StringT(begin_, end_);
			}

			void print(int level = 0) {
				print_spacing(level);
				cout << to_string();
			}


			virtual bool match() {
				return match(this->begin(), this->end());
			}

			//virtual bool match(IteratorT& begin, IteratorT& end)  { return false;};
			virtual bool match(IteratorT begin, IteratorT end) {
				if (begin == end)
					return false;

				this->begin(begin);
				this->end(end);
				bool ret = match(begin);
				//begin = this->begin();
				//end = this->end();
				return ret;
				//return end;
			}

			virtual bool match(IteratorT begin) {
				IteratorT next = begin;
				if (!detect(begin))
					return false;

				next = begin;
				begin_notify(begin);
				return match_rest(next);
			}

			virtual bool resume_match(IteratorT begin, IteratorT end) {
				if (this->begin() != begin) {
					this->end(end);
					bool ret = match_rest(begin);
					//end = this->end();
					return ret;
				}

				return match(begin, end);
			}

			virtual bool detect(IteratorT& begin) {
				skip_not_valid_char(begin);

				//this->end(next_char);
				/*
				 * the begining of the entity should be decided in the is_start function
				 */
				while (!is_start(begin) && !this->eow(begin))
					begin++;

				if (this->eow(begin))
					return false;

				return true;
			}

			virtual bool equal(StringT what) {
				return to_string() ==  what;
			}

			friend bool operator == (const StringBound& left, const StringBound& right) {
				return left.to_string() == right.to_string();
			}

			virtual void create(StringT text="") {
				add_start(text);
				add_content(text);
				add_end(text);
			}

			void ref_erase() {
				if (this->ref().length() > 0)
					ref().erase();
			}

			virtual void flush(int level=0) {}

			StringT& ref() { return content_; }

			virtual bool is_valid_char(IteratorT it) {
				return true;
			}

		protected:
			virtual void begin_notify(IteratorT& begin) {
			}

			virtual void end_notify(IteratorT& end) {
			}

			bool match_rest(IteratorT next_char) {
				//IteratorT next_char = begin;
				// in some cases,the last char cannot be set due to reach end of the string stream
				// so test end of stream has to be done inside of is_end function or right hand side of it.
				while (!this->eow(next_char) && /* && */!is_end(next_char))
					next_char++;
				this->end(next_char);
				end_notify(next_char);
				return !(this->begin() == this->end());
			}

			void print_spacing(int level) {
				for (int i=0; i<level; i++)
					cout << SPACING;
			}

			virtual bool is_start(IteratorT& it) {
				bool ret = !eow(it);
				if (ret)
					this->begin(it);
				return ret;
			}

			virtual bool is_end(IteratorT& it) {
				return eow(it);
			}

			virtual void add_start(StringT& text) {
			}

			virtual void add_content(StringT& text) {
				ref().append(text);
			}

			virtual void add_end(StringT& text) {;
			}

			unsigned long long int offset() {
				return offset_;
			}

		private:
			void init() {
				//detected_ = false;
				//content_ref_ = content_;
			}

		private:
			StringT							content_;
			//StringT&						content_ref_;

			IteratorT 						begin_;
			IteratorT 						end_;
			unsigned long long int 			offset_;	 // used in creating the StringBound content
	};

	template <typename EntityT,  typename ContainerT = std::vector<EntityT *> >
	//template <typename EntityT, typename ContainerT>
	class Entity {
		private:
			typedef EntityT								*EntityPtr;
			//typedef std::vector<EntityPtr>			ContainerT;

		public:
			typedef	EntityT								entity_type;
			typedef EntityPtr							container_entity_type;
			typedef typename ContainerT::iterator 		iterator;
			typedef iterator 							entity_iterator;

		protected:
			ContainerT 									children_;
			iterator									current_pos_;

		public:
			Entity() { current_pos_ = children_.begin(); }
			~Entity() {
				clear();
			}

			//void add(EntityT entity) { children_.push_back(entity); }
			bool more() {
				if (children_.size() > 0) {
					if (current_pos_ != children_.end())
						return true;
				}
				return false;
			}

			iterator iter_begin() { return children_.begin(); }
			iterator iter_end()  { return children_.end(); }

			void reset() { current_pos_ = children_.begin(); }

			iterator next() { return current_pos_++; }

			iterator operator[] (int index) {
				if (index >= static_cast<int>(size() - 1))
					return children_[size() - 1];
				return children_[index];
			} //non-const

			const iterator operator[] (int index) const {
				if (index >= (size() - 1))
					return children_[size() - 1];
				return children_[index];
			} //const

			size_t size() { return children_.size(); }

			void print(int level = 0) {
				iterator it = children_.begin();
				for (;
					it != children_.end();
					++it)
				{
					(*it)->print(level + 1);
					std::cout << std::endl;
				}
				//std::for_each(children_.begin(), children_.end(), mem_fun(&Entity::print));
			}

			ContainerT& children() { return children_; }

			void add(EntityT *entity_ptr) {
				 children_.push_back(entity_ptr);
			}

			void clear() {
				iterator it;
				for (it = children_.begin(); it != children_.end(); it++) {
					if (*it) {
						delete *it;
						(*it) = NULL;
					}
				}
				children_.clear();
			}

			friend bool operator == (const Entity& left, const Entity& right) {
				return false;
			}
	};

	//template <typename StringT = std::string, typename IteratorT = typename StringT::iterator,
	//typename EntityT = StringBound<StringT, IteratorT>/*,  typename ContainerT = std::vector<EntityT>*/ >
	template <
		typename EntityT = StringBound<> >
	class StringEntity : public StringBound<typename EntityT::string_type, typename EntityT::iterator>
							, public Entity<EntityT> {
		private:
			typedef typename EntityT::string_type 				StringT;
			typedef typename EntityT::iterator					IteratorT;

		public:
			typedef	StringT										string_type;
			typedef IteratorT									iterator;
			typedef	EntityT										entity_type;
			typedef typename Entity<EntityT>::entity_iterator 	entity_iterator;

		public:
			StringEntity() : StringBound<StringT, IteratorT>::StringBound()
				, Entity<EntityT>::Entity() {}
			StringEntity(IteratorT it) :
				StringBound<StringT, IteratorT>::StringBound(it)
				, Entity<EntityT>::Entity()  {
			}
			StringEntity(IteratorT begin, IteratorT end) :
				StringBound<StringT, IteratorT>::StringBound(begin, end)
					, Entity<EntityT>::Entity() {}
			StringEntity(StringT content) :
				StringBound<StringT, IteratorT>::StringBound(content)
				, Entity<EntityT>::Entity()  {
			}

			virtual ~StringEntity() {}

			virtual entity_iterator find(StringT what) {
				entity_iterator begin = this->iter_begin();
				for (; begin != this->iter_end(); begin++)
					if ((*begin)->equal(what))
						break;
				return begin;
			}

//			virtual entity_iterator find_entity(EntityT& entity) {
//				return std::find(this->iter_begin(), this->iter_end(), entity);
//			}
	};
}

#include "stpl_entity.tcc"

#endif /*STPL_ENTITY_H_*/
