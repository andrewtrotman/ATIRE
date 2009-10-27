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

#ifndef STPL_XML_ENTITY_H_
#define STPL_XML_ENTITY_H_

#include <sstream>
#include <cassert>
#include <map>
#include <list>

#include "stpl_xml_basic.h"
#include "../stpl_property.h"
#include "../characters/stpl_character.h"


namespace stpl {
	namespace XML {

		template <typename StringT = std::string, typename IteratorT = typename StringT::iterator>
		class Text: public BasicXmlEntity<StringT, IteratorT>
		{
			public:
				typedef	StringT	string_type;
				typedef IteratorT	iterator;

			private:
				void init() { this->type_ = TEXT; }

			public:
				Text() : BasicXmlEntity<StringT, IteratorT>::BasicXmlEntity() { init(); }
				Text(IteratorT it)
					 : BasicXmlEntity<StringT, IteratorT>::BasicXmlEntity(it) { init(); }
				Text(IteratorT begin, IteratorT end)
					 : BasicXmlEntity<StringT, IteratorT>::BasicXmlEntity(begin, end) { init(); }
				Text(StringT content) :
					BasicXmlEntity<StringT, IteratorT>::BasicXmlEntity() {
					init();
					this->create(content);
				}
				virtual ~Text() {}

				virtual void flush(int level=0) {
					StringT indent(level*2, ' ');

					this->ref().insert(0, indent);
				}

			protected:
				virtual bool is_start(IteratorT& it) {
					skip_whitespace(it);
					this->begin(it);
					return true;
				}

				virtual bool is_end(IteratorT& it) {
					return this->eow(it) || text_stop(it);
				}

				virtual bool text_stop(IteratorT next) {
					return BasicXmlEntity<StringT, IteratorT>::is_start_symbol(next);
				}

				virtual void add_content(StringT& text) {
					//this->set_begin(0);
					this->ref().append(text);
					//this->set_end(text.length());
				}
		};


		template <typename StringT = std::string,
			 typename IteratorT = typename StringT::iterator,
			 typename AttributeT = XmlAttribute<StringT, IteratorT> >
		class ElemTag: public XmlKeyword<StringT, IteratorT>
		{
			public:
				typedef	StringT	string_type;
				typedef IteratorT	iterator;
				typedef typename AttributeT::attributes_type	attributes_type;

			private:
				StringBound<StringT, IteratorT> name_;

			protected:
				attributes_type attributes_;

			private:
				void init() { this->type_ = LABEL; }

			public:
				ElemTag() : XmlKeyword<StringT, IteratorT>::XmlKeyword() {}
				ElemTag(IteratorT it) : XmlKeyword<StringT, IteratorT>::XmlKeyword(it), name_(it, it)  {
					init();
				}
				ElemTag(IteratorT begin, IteratorT end) :
					 XmlKeyword<StringT, IteratorT>::XmlKeyword(begin, end), name_(begin, begin) { init(); }
				//ElemTag(StringT content) {
				//	XmlKeyword<StringT, IteratorT>::XmlKeyword(content);
				//	init();
				//}
				ElemTag(StringT content) :
					XmlKeyword<StringT, IteratorT>::XmlKeyword(content) {
					init();
				}
				virtual ~ElemTag() {
					clear();
				};

				ElemTag& operator= (ElemTag& elem_k) {
					this->clone(reinterpret_cast<XmlKeyword<StringT, IteratorT>*>(&elem_k));
					return *this;
				}

				ElemTag& operator= (const ElemTag* elem_k_ptr) {
					this->clone(reinterpret_cast<XmlKeyword<StringT, IteratorT>*>(elem_k_ptr));
					return *this;
				}

				ElemTag& operator= (XmlKeyword<StringT, IteratorT>* elem_k_ptr) {
					this->clone(elem_k_ptr);
					return *this;
				}

				std::pair<bool, StringT> attribute(const StringT &attr) const
				{
					//if (this->attributes_.size() > 0) {
						//typename std::map<StringT, StringT>::const_iterator i = this->attributes_.find(attr);
						/*
						typename attributes_type::const_iterator i = this->attributes_.find(attr);
						if (i != this->attributes_.end()) {
							return make_pair(true, i->second->value());
						}
						*/
					//}
						/* sorry, I just cannot find the thing I want using map.find()*/
						typename attributes_type::const_iterator	it = attributes_.begin();
						for (; it!= attributes_.end(); it++) {
							if (it->first == attr)
								return make_pair(true, it->second->value());
						}

					return make_pair(false, StringT());
				}

				/*
				 *  defines valid characters for tag name
				 *  initially, all readable chars are valid except whitespace
				 */
				static bool is_valid_name_char(IteratorT it) {
					if (isspace(*it) || iscntrl(*it)
							|| (BasicXmlEntity<StringT, IteratorT>::XML_KEY_CLOSE == *it)
							|| (BasicXmlEntity<StringT, IteratorT>::XML_KEY_OPEN == *it))
						return false;
					else
						return true;
				}

				StringBound<StringT, IteratorT>& name() {
					if (name_.length() == 0) {
						// TODO assert it is the end of the ElemTag already
						IteratorT it = this->body_.begin();
						name_.begin(it);

						while(isalnum(*it) && it != this->end())
							it++;
						name_.end(it);
					}
					//return StringT(name_.begin(), name_.end());
					return name_;
				}

				virtual bool match(IteratorT begin, IteratorT end) {
					if (XmlKeyword<StringT, IteratorT>::match(begin, end)) {
						if (this->type_ == LABEL)
							return true;
					}
					return false;
				}

				virtual void print(int level = 0) {
					name().print(level);
					print_attributes(level);
					std::cout << endl;
				}

				void print_attributes(int level) {
					if (attributes_.size() > 0) {
						std::cout << "(";
						typename attributes_type::iterator	it = attributes_.begin();
						for (; it!= attributes_.end(); it++) {
							std::cout << it->second->name()
									<<	":"
									<<  it->second->value()
							        << " ";
						}
						std::cout << ")" << std::endl;
					}
				}

				void parse_attributes() {
					IteratorT begin = name_.end;
					IteratorT end = this->end();
					while (parse_attribute(begin, end)) {
						begin = end;
						end = this->end();
					}
				}

				virtual bool required_end_tag() {
					return true;
				}

				virtual bool make_following_element_as_child(ElemTag* next_tag_ptr) {
					return false;
				}

				virtual bool force_close(ElemTag* next_tag_ptr) {
					return false;
				}

				void new_attribute(StringT name, StringT value) {
					AttributeT* attr_ptr = new AttributeT();
					attr_ptr->create(name, value);
					attributes_.insert(make_pair(name, attr_ptr));
				}

				virtual void flush(int level=0) {
					StringT indent(level*2, ' ');

					this->ref().insert(0, indent);

					if (attributes_.size() > 0) {
						typename attributes_type::const_iterator	it = attributes_.begin();
						for (; it!= attributes_.end(); it++) {

							std::string::size_type pos = this->ref().rfind(BasicXmlEntity<StringT, IteratorT>::XML_KEY_CLOSE);
							if (pos != std::string::npos) {
								StringT temp(" ");
								temp.append(it->second->ref());
								this->ref().insert(pos, temp);
							}
						}
					}
				}

			private:
				void clear() {
					if (attributes_.size() > 0) {
						typename attributes_type::const_iterator it = attributes_.begin();
						for (; it!= attributes_.end(); it++) {
							delete it->second;
						}
						attributes_.clear();
					}
				}

				bool parse_attribute(IteratorT& begin, IteratorT& end) {
					this->skip_not_valid_char(begin);
					AttributeT* attr_ptr = new AttributeT(begin, end);
					bool ret = false;
					if ((ret = attr_ptr->match(begin, end))) {
						attributes_.insert(make_pair(attr_ptr->name(), attr_ptr));
						end = attr_ptr->end();
					}
					if (!ret) {
						delete attr_ptr;
						return false;
					}
					return ret;
				}

			protected:

				virtual bool is_start(IteratorT& it) {
					if (XmlKeyword<StringT, IteratorT>::is_start(it) && this->type_ == LABEL) {
						it = this->body_.begin();
						name_.begin(it);
						while (!this->eow(it) && is_valid_name_char(it))
							++it;
						name_.end(it);
						return true;
					}
					return false;
				}

				virtual bool is_end(IteratorT& it) {
					if (!XmlKeyword<StringT, IteratorT>::is_end(it)) {
						IteratorT begin = it;
						IteratorT end = this->end();
						if (parse_attribute(begin, end)) {
							it = end;
							if (XmlKeyword<StringT, IteratorT>::is_end(it)) {
								this->body_.end(it);
								return true;
							}
						}
						//if (!ret)
						//	; //it = ++end; /// since the end char(">") is not included in attribute, when coming to end of parsing attributes
										/// that means we need step one char ahead
						//else
						//	it = --end; // if the end is the end char already, but it will be skipped by the parent function
						return false;
					}

					return true;
				}

				virtual void add_content(StringT& text) {
					add_name(text);
					//add_attributes(text);
				}

				void add_name(StringT& name) {
					// debug
					///cout << "add name for elemtag " << endl;
					this->ref().append(name);
					//IteratorT begin = this->ref().end() - name.length();
					//name_.set_begin(this->ref().length());
					//name_.set_end(name.length());
				}
		};


		template <typename StringT = std::string,
							typename IteratorT = typename StringT::iterator
						  >
		class InfoNode : public XmlKeyword<StringT, IteratorT>
		{
			public:
				typedef	StringT	string_type;
				typedef IteratorT	iterator;

			private:
				void init() { this->type_ = INFO; }

			public:
				InfoNode() : XmlKeyword<StringT, IteratorT>::XmlKeyword() {}
				InfoNode(IteratorT it) :
					XmlKeyword<StringT, IteratorT>::XmlKeyword(it) {
					init();
				}
				InfoNode(IteratorT begin, IteratorT end) :
					XmlKeyword<StringT, IteratorT>::XmlKeyword(begin, end) { init(); }
				InfoNode(StringT content) :
					XmlKeyword<StringT, IteratorT>::XmlKeyword(content) {
					init();
				}
				virtual ~InfoNode() {};

				virtual bool match(IteratorT begin, IteratorT end) {
					if (XmlKeyword<StringT, IteratorT>::match(begin, end)) {
						if (this->type_ == INFO)
							return true;
					}
					return false;
				}
		};

		template <typename StringT = std::string,
							typename IteratorT = typename StringT::iterator
						  >
		class Comment : public XmlKeyword<StringT, IteratorT>
		{
			public:
				typedef	StringT	string_type;
				typedef IteratorT	iterator;

			private:
				void init() { this->type_ = COMMENT; }

			public:
				Comment()  :
					XmlKeyword<StringT, IteratorT>::XmlKeyword(){}
				Comment(IteratorT it) :
					XmlKeyword<StringT, IteratorT>::XmlKeyword(it)  {
					init();
				}
				Comment(IteratorT begin, IteratorT end) :
					XmlKeyword<StringT, IteratorT>::XmlKeyword(begin, end) { init(); }
				Comment(StringT content) :
					XmlKeyword<StringT, IteratorT>::XmlKeyword(content) {
					init();
				}
				virtual ~Comment() {};


				Comment& operator= (XmlKeyword<StringT, IteratorT>* elem_k_ptr) {
					this->clone(elem_k_ptr);
					return *this;
				}

				virtual bool match(IteratorT begin, IteratorT end) {
					if (XmlKeyword<StringT, IteratorT>::match(begin, end)) {
						if (this->type_ == COMMENT)
							return true;
					}
					return false;
				}
		};


		template <typename StringT = std::string, typename IteratorT = typename StringT::iterator>
		class DocType: public XmlKeyword<StringT, IteratorT>
		{
			public:
				typedef	StringT	string_type;
				typedef IteratorT	iterator;

			private:
				void init() { this->type_ = DTD; }

			public:
				DocType() : XmlKeyword<StringT, IteratorT>::XmlKeyword() {
					init();
				}
				DocType(IteratorT it) :
					XmlKeyword<StringT, IteratorT>::XmlKeyword(it){
					init();
				}
				DocType(IteratorT begin, IteratorT end) :
					XmlKeyword<StringT, IteratorT>::XmlKeyword(begin, end){
					init();
				}
				DocType(StringT content) :
					XmlKeyword<StringT, IteratorT>::XmlKeyword(content) {
					init();
				}
				virtual ~DocType() {}

				virtual bool match(IteratorT begin, IteratorT end) {
					if (XmlKeyword<StringT, IteratorT>::match(begin, end)) {
						if (this->type_ == DTD)
							return true;
					}
					return false;
				}
		};

		template <typename StringT = std::string
			, typename IteratorT = typename StringT::iterator>
		class XmlNodeTypes {
			public:
				typedef BasicXmlEntity<StringT, IteratorT> 				basic_entity;
				typedef XmlKeyword<StringT, IteratorT>					keyword_type;
				typedef ElemTag<StringT, IteratorT> 					tag_type;
				typedef Text<StringT, IteratorT> 						text_type;
				typedef Entity<basic_entity>							container_type;
				typedef typename container_type::container_entity_type	container_entity_type;
		};

		template <typename StringT = std::string
							, typename IteratorT = typename StringT::iterator
							, typename NodeTypesT = XmlNodeTypes<StringT, IteratorT>
						  >
		class Element: public NodeTypesT::basic_entity
								 , public NodeTypesT::container_type
		{
			private:
				typedef typename NodeTypesT::text_type 							TextT;
				typedef typename NodeTypesT::tag_type							ElemTagT;
				typedef typename NodeTypesT::keyword_type						XmlKeywordT;

			public:
				typedef	StringT													string_type;
				typedef IteratorT												iterator;
				typedef Comment<StringT, IteratorT>								comment_type;
				typedef typename NodeTypesT::basic_entity 						basic_entity;
				typedef typename NodeTypesT::container_type						container_type;
				typedef typename container_type::entity_iterator				entity_iterator;
				typedef list<
					typename container_type::container_entity_type
							>													tree_type;
				typedef typename std::map<StringT, bool>						ie_map; /// include or exclude map

			protected:
				typedef StringBound<StringT, IteratorT> 						StringB;
				StringB 														body_;

				ElemTagT* start_k_;
				ElemTagT* end_k_;
				ElemTagT* last_tag_ptr_;

				//Element* parent_;
				StringT xpath_;

			private:
				void init() {
					last_tag_ptr_ = NULL;
					start_k_ = NULL;
					end_k_ = NULL;
					body_.begin(this->begin());
					body_.end(this->begin());
					this->type(ELEMENT);
				}

				void cleanup() {
					if (start_k_)
						delete start_k_;
					if (end_k_)
						delete end_k_;
					cleanup_last_tag();
				}

				void cleanup_last_tag() {
					if (last_tag_ptr_) {
						delete last_tag_ptr_;
						last_tag_ptr_ = NULL;
					}
				}

			public:
				Element() : BasicXmlEntity<StringT, IteratorT>::BasicXmlEntity()
							, Entity<BasicXmlEntity<StringT, IteratorT> >::Entity() { init(); }
				Element(IteratorT it) :
					BasicXmlEntity<StringT, IteratorT>::BasicXmlEntity(it)/*, start_k_(it, it)*/
					, Entity<BasicXmlEntity<StringT, IteratorT> >::Entity() { init(); }
				Element(IteratorT begin, IteratorT end) :
					BasicXmlEntity<StringT, IteratorT>::BasicXmlEntity(begin, end)/*, start_k_(begin, begin)*/
					, Entity<BasicXmlEntity<StringT, IteratorT> >::Entity() { init(); }
				Element(StringT name) :
					BasicXmlEntity<StringT, IteratorT>::BasicXmlEntity()
					, Entity<BasicXmlEntity<StringT, IteratorT> >::Entity() {
					init();
					this->create(name);
				}
				virtual ~Element() {
					cleanup();
				}

				void init(IteratorT begin, IteratorT end) {
					this->begin(begin);
					this->end(end);
					//start_k_.begin(begin);
					//start_k_.end(end);
				}

				void set_start_keyword(ElemTagT* start_k) {
					start_k_ = start_k;
				}

				void set_last_tag(ElemTagT* last_tag_ptr) {
					last_tag_ptr_ = last_tag_ptr;
				}

				std::pair<bool, StringT> attribute(const StringT &attr) const {
					if (start_k_)
						return start_k_->attribute(attr);
					return make_pair(false, StringT());
				}
				//void set_parent(Element* parent) {
				//	parent_ = parent;
				//}

			protected:
				virtual bool is_last_tag_end_tag() {
					if (last_tag_ptr_/*&& last_tag_ptr_->length() > 0*/) {
						if (last_tag_ptr_->is_end_xml_keyword()) {
							if (start_k_) {
								StringT	last_tag_name(last_tag_ptr_->name().begin(), last_tag_ptr_->name().end());
								StringT start_k_name(start_k_->name().begin(), start_k_->name().end());
								if (last_tag_name == start_k_name) {
									// TODO assert elem_k is closed elem
									end_k_ = last_tag_ptr_;
									last_tag_ptr_ = NULL;
									IteratorT end = end_k_->begin();
									body_.end(end);

									this->end(end_k_->end());
								} else {
									// TODO error message here for XML
									// but could be alright for html
									if (this->parent() && this->parent()->is_element()) {
										reinterpret_cast<Element*>(this->parent())->set_last_tag(last_tag_ptr_);
										IteratorT end = last_tag_ptr_->begin();
										body_.end(end);
										this->end(end);
										last_tag_ptr_ = NULL;
									}
									else {
									// there may be error here,since the Element is not opened
									// but the parser should be error-tolorant with HTML
										return false;
									}
								}
							} else {
								// TODO error message for not opening the tag yet
								end_k_ = last_tag_ptr_;
								last_tag_ptr_ = NULL;
								IteratorT begin = end_k_->begin();
								body_.begin(begin);
								body_.end(begin);
								this->end(end_k_->end());
							}
							return true;

						} else { // if last_tag is not a end tag
							bool ret = false;
							if (this->start_k_) {
								// if the end tag is optional or fibidden
								// then it is the end
								if (!this->start_k_->required_end_tag())
									ret = true;

								if (ret && this->start_k_->make_following_element_as_child(last_tag_ptr_))
									ret = false;

								if (!ret && this->start_k_->force_close(last_tag_ptr_))
									ret = true;
							} //else {
								//ret = true;
							//}

							if (ret) {
								IteratorT end = last_tag_ptr_->begin();
								if (this->parent() && this->parent()->is_element()) {
									reinterpret_cast<Element*>(this->parent())->set_last_tag(last_tag_ptr_);
									last_tag_ptr_ = NULL;
								}
								body_.end(end);
								this->end(end);
							}
							return ret;
						}
						return false;
					}
					return true;
				}

				virtual bool is_start(IteratorT& it) {
					bool ret = false;
					if (start_k_ && start_k_->length() > 0) {
						ret = true;
					}
					else {
						IteratorT begin = this->begin();
						IteratorT end = this->end();

						ret = assign_start_tag(begin, end);
					}

					if (ret) {
						this->type_ = ELEMENT;
						assert(start_k_);
						this->begin(start_k_->begin());
						body_.begin(start_k_->end());
					}
					return ret;
				}

				virtual bool is_end(IteratorT& it) {
					bool ret = false;
					if (start_k_) {
						if (start_k_->is_ended_xml_keyword())
							ret = true;

						if (start_k_->is_end_xml_keyword()) {
							// may print error message here
							// since the Element is closed without opening it
							ret = true;
						}

						if (ret) {
							body_.end(start_k_->end());
							return ret;
						}
					}

					if (!last_tag_ptr_) {
						match_text(it);
						//IteratorT end = this->end();

						// if it is the end of character stream
						if (eow(it)) {
							//TODO error message here for xml
							// but could be valid for html
							body_.end(it);
							this->end(it);
							return true;
						}

						// cleanup_last_tag();
						// skip non valid char or get next tag
						skip_not_valid_char(it);
					}

					if (!last_tag_ptr_ || is_last_tag_end_tag()) {
 						it = this->end();
 						return true;
 					}

 					assert(last_tag_ptr_);

					IteratorT end = this->end();
					IteratorT begin = last_tag_ptr_->begin();
					IteratorT new_begin = last_tag_ptr_->end();
 					Element* child = new Element(begin, end);
					child->set_parent(reinterpret_cast<basic_entity* >(this));
					child->set_start_keyword(last_tag_ptr_);
					child->content().begin(last_tag_ptr_->end());
					last_tag_ptr_ = NULL;

					if (child->resume_match(new_begin, end) && child->length() > 0) {
						this->add(child);

						//if (last_tag_ptr_ && is_last_tag_end_tag()) {
						//	it = this->end();
						//	return true;
						//}
						// process the text after the child
						// match_text(it = end);
						if (last_tag_ptr_)
							it = last_tag_ptr_->end();
						else
							it = child->end();
						// because it will return false, that means it will move ahead one character
						// in the parent "match" function, and we don't want to miss the current possition
						// that is why --it
						--it;
						return false;
					}
					// TODO someting must go wrong here, output error message
					it = begin; // where the error happens
					body_.end(it);
					delete child;
					return true;

					//--it;
					//return false;
					// TODO something wrong happens here
					//return true;
				}

				/*
				void get_next_tag(IteratorT it) {
					if (!last_tag_ptr_) {
						last_tag_ptr_ = new ElemTagT(it, it);
						bool ret = skip_comment_unknown_node(last_tag_ptr_, it);
						if (!ret) {
							cleanup_last_tag();
							return true;
						}
					}
				}
				*/
				bool assign_start_tag(IteratorT begin, IteratorT end) {
					bool ret = false;
					if (!last_tag_ptr_) {
						if (!start_k_)
							start_k_ = new ElemTagT(begin, end);
						ret = start_k_->match(begin, end);
					}
					else {
						ret = true;
						start_k_ = last_tag_ptr_;
						last_tag_ptr_ = NULL;
					}
					return ret;
				}

				virtual IteratorT skip_not_valid_char(IteratorT& it) {
					skip_whitespace(it);

					//if (!start_k_)
					//	start_k_ = new ElemTagT(next, next);

					//while (!this->eow(next) && start_k_->length() <= 0 ) {
					//	if (!skip_comment_unknown_node(start_k_, next))
					//		break;
					//}
					if (!last_tag_ptr_) {
						last_tag_ptr_ = new ElemTagT(it, it);

						bool ret = false;
						while (!this->eow(it) && last_tag_ptr_->length() <= 0 )
							if (!(ret = skip_comment_unknown_node(last_tag_ptr_, it))) {
								cleanup_last_tag();
								break;
							}
					}
					return it;
				}

				// comments could be in any place, like before, after or anywhere in the middle of a Element
				bool skip_comment_unknown_node(ElemTagT* tag_ptr, IteratorT& begin) {
					bool ret = true;

					IteratorT orig_begin = begin;
					IteratorT end = this->end();
					XmlKeywordT* keyword_ptr = new XmlKeywordT(begin, end);

					if (keyword_ptr->detect(begin)) {
						//begin = keyword_ptr->end();
						IteratorT new_begin = keyword_ptr->content().begin();
						if (keyword_ptr->type() == COMMENT) {
							comment_type* node_ptr = new comment_type(orig_begin, end);
							node_ptr->set_parent(reinterpret_cast<basic_entity* >(this));
							node_ptr->clone(keyword_ptr);
							//node_ptr->detected(true);
							if ((ret = node_ptr->resume_match(new_begin, end))) {
								begin = node_ptr->end();
								add(reinterpret_cast<basic_entity* >(node_ptr));
							}

							delete keyword_ptr;
							keyword_ptr = NULL;
						}
						else if (keyword_ptr->type() == LABEL) {
							tag_ptr->clone(keyword_ptr);
							//tag_ptr->detected(true);
							if ((ret = tag_ptr->match(orig_begin, end)))
								begin = tag_ptr->end();

							delete keyword_ptr;
							keyword_ptr = NULL;
						}
						else {
							keyword_ptr->set_parent(reinterpret_cast<basic_entity* >(this));
							//keyword_ptr>detected(true);
							if ((ret = keyword_ptr->resume_match(new_begin, end))) {
								begin = keyword_ptr->end();
								add(reinterpret_cast<basic_entity* >(keyword_ptr));
							}
						}
					} else {
						delete keyword_ptr;
						ret = false;
					}

					return ret;
				}

				virtual void match_text(IteratorT& next) {
					IteratorT end = this->end();
					TextT* text = new TextT(next, end);
					text->set_parent(reinterpret_cast<basic_entity* >(this));
					if (text->match(next, end))	{
						this->add(reinterpret_cast<basic_entity*>(text));
						next = text->end();
					}
					else
						delete text;
				}

				virtual void add_start(StringT& text) {
					// debug
					// this->ref().append(text + " element\n");
					//cout << "add start from element ..." << endl;
					this->add_start_tag(text);
				}

				virtual void add_content(StringT& text) {
					//add_children(text);
				}

				virtual void add_end(StringT& text) {
					this->add_end_tag(text);
				}

				void add_start_tag(StringT& text) {
					if (start_k_)
						delete start_k_;

					start_k_ = new ElemTagT();
					//assert(start_k_->ref() == this->ref());
					start_k_->create(text);

				}

				void add_end_tag(StringT& text) {
					if (end_k_)
						delete end_k_;

					end_k_ = new ElemTagT();
					//start_k_->ref(this->ref());
					end_k_->set_end_xml_keyword(true);
					end_k_->create(text);
				}

				/*
				void add_child(basic_entity* child) {
					IteratorT begin = this->ref().begin();
					if (start_k_)
						begin = start_k_->end();

					if (this->children().size() > 0)
						begin = (this->children().back())->end();

					this->ref().insert(begin - this->ref().begin(), child->to_string());
					this->add(child);
				}
				*/

			public:
				virtual StringT name() {
					if (start_k_)
						return StringT(start_k_->name().begin(), start_k_->name().end());
					return StringT("");
				}

				virtual void print(int level = 0) {
					print_tag(level);
					//print_text(level);
					std::cout << std::endl;
					if (this->size() > 0) {
						print_childrent(level + 1);
						std::cout << std::endl;
					}
				}

				void print_attributes(int level = 0) {
					if (start_k_)
						start_k_->print_attributes(level);
				}

				void text(StringT& text) {
					ie_map nm;
					this->text(text, false, nm, true);
				}

				void all_text(StringT& text) {
					ie_map nm;
					this->text(text, true, nm, true);
				}

				void all_text(StringT& text
						, ie_map& nm
						, bool sub_text = false
						, bool force = false) {
					this->text(text, true, nm, sub_text, force);
				}

//				void all_text_exclude(StringT& text, ie_map& nm) {
//					this->text(text, true, nm, true);
//				}
//
//				void all_text_include(StringT& text, ie_map& nm) {
//					this->text(text, true, nm, false);
//				}

				StringT text() {
					StringT text;
					this->text(text);
					return text;
				}

				StringT all_text() {
					StringT text;
					this->all_text(text);
					return text;
				}

				void traverse(tree_type& tree) {
					entity_iterator it;
					for (it = this->iter_begin(); it != this->iter_end(); ++it) {
					//this->reset();
					//while (this->more()) {
					//	entity_iterator it = this->next();
						tree.push_back(*it);
						if ((*it)->is_element()) {
							reinterpret_cast<Element*>(*it)->traverse(tree);
						}
					}
				}

				void find_children(tree_type& tree) {
					find_children("", tree, true);
				}

				void find_children(StringT name,tree_type& tree) {
					find_children(name, tree, false);
				}

				void find(StringT name, tree_type& tree) {
					if (this->name() == name) {
						tree.push_back(this);
					}
					else {
						find_children(name, tree, false);
					}
					return;
				}

				Element* find_child(StringT child_name) {
					entity_iterator it;
					for (it = this->iter_begin(); it != this->iter_end(); ++it) {
						if ((*it)->is_element()) {
							Element* child = reinterpret_cast<Element*>(*it);
							StringT name = child->name();
							// debug
							// cout << "comparing with " << name << endl;
							if (name == child_name)
								return child;
							else
								child->find_child(child_name);
						}
					}
					return NULL;
				}

				virtual StringB& content() {
					return body_;
				}

				void name(StringT name) {
					if (start_k_) {
						delete start_k_;
					}
				}

				void new_text(StringT text) {
					TextT* text_ptr = new TextT(text);
					this->add(reinterpret_cast<basic_entity*>(text_ptr));
				}

				void new_child(Element* child_ptr) {
					this->add(reinterpret_cast<basic_entity*>(child_ptr));
				}

				void new_attribute(StringT name, StringT value) {
					if (start_k_)
						start_k_->new_attribute(name, value);
				}

				virtual void flush(int level=0) {
					StringT indent(level*2, ' ');
					bool n_flag = false;

					this->ref().erase();

					//if (level > 0)
					//	this->ref().append(indent);

					if (start_k_) {
						start_k_->flush(level);
						this->ref().append(start_k_->ref());
					}

					entity_iterator it;
					for (it = this->iter_begin(); it != this->iter_end(); ++it) {
						this->ref().append("\n");

						(*it)->flush(level+1);
						this->ref().append((*it)->ref());

						this->ref().append("\n");

						n_flag = true;
					}

					//if (n_flag && level > 0)
					//	this->ref().append(indent);

					if (end_k_ && !start_k_->is_ended_xml_keyword()) {
						end_k_->flush(level);
						this->ref().append(end_k_->ref());
					}
				}

				void set_empty() {
					if (start_k_)
						start_k_->set_ended_xml_keyword(true);
				}

				StringT xpath() { return xpath_; }
				void xpath(StringT xpath) { xpath_ = xpath; }

			private:
				/**
				 * all_text is true for including children' texts
				 */
				void text(StringT& text
						, bool all_text
						, ie_map& nm
						, bool sub_text = false
						, bool force = false) {

					if (nm.size() > 0 &&
						(!sub_text || (force && sub_text))) {
						//static_cast<Element*>(*it)->text(text, all_text, nm, sub_text, force);
						//else  {

						//Element* tmp_elem_ptr = static_cast<Element*>((*it));

						typename ie_map::iterator ie_node = nm.find(this->name());
						bool found = ie_node != nm.end();

						if (found) {
							if (!(ie_node->second))
								return;
							else
								sub_text = true;
								//static_cast<Element*>(*it)->text(text, all_text, nm, true, force);
						}
						//else
						//	sub_text = false;
							//static_cast<Element*>(*it)->text(text, all_text, nm, false, force);
					}

					this->reset();
					while (this->more()) {
						entity_iterator it = this->next();

						if ((*it)->type() == TEXT) {

							if (sub_text || (nm.size() == 0)) {
								if (text.length() > 0)
									text.append("\n");
								text.append((*it)->begin(), (*it)->end());
							}
						}
						else if ((*it)->is_element() && all_text) {
							static_cast<Element*>(*it)->text(text, all_text, nm, sub_text, force);
						}
					}

				}

				virtual void print_tag(int level) {
					if (start_k_)
						start_k_->print(level);
				}

				void print_text(int level) {
					std::cout << " - " ;
					this->reset();
					while (this->more()) {
						entity_iterator it = this->next();
						if ((*it)->type() == TEXT) {
							(*it)->print();
						}
					}
				}

				void print_childrent(int level) {
					this->reset();
					while (this->more()) {
						entity_iterator it = this->next();
						if ((*it)->type() == ELEMENT) {
							reinterpret_cast<Element*>(*it)->print(level + 1);
						} else {
							(*it)->print(level + 1);
						}
					}
				}


				// find children elements with child_name
				// if child name is empty, then return all children
				void find_children(StringT child_name, tree_type& tree, bool all) {
					//int count = 0;
					entity_iterator it;
					for (it = this->iter_begin(); it != this->iter_end(); ++it) {
						if ((*it)->is_element()) {
							Element* child = reinterpret_cast<Element*>(*it);
							StringT name = child->name();
							// debug
							// cout << "comparing with " << name << endl;
							if (name == child_name || all) {
								// create the most simple xpath
								std::ostringstream oss;
								oss << "//" << name << "[" << (tree.size()) << "]";
								child->xpath(oss.str());

								tree.push_back(*it);
							}
							else
								child->find(child_name, tree);
						}
					}
					return;
				}
		};
	}
}

#endif /*STPL_XML_ENTITY_H_*/
