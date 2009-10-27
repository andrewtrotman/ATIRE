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

#ifndef STPL_HTML_ENTITY_H_
#define STPL_HTML_ENTITY_H_

#include "../xml/stpl_xml_entity.h"
#include "../utils/icstring.h"

/**
 * this file is implemented based on HTML specification 4.01,
 * A better solution could be loading the DTD file of the HTML 
 * but a simpler way to do the job would be like the implementation of this file
 * at the moment
 */
namespace stpl {
	namespace HTML {
		//using namespace XML;
		//typedef icstring String;
		//const String HTML_START_TAG("<HTML>");
			
		template <typename StringT = icstring, typename IteratorT = typename StringT::iterator>
		class Text: public XML::Text<StringT, IteratorT> 
		{
			public:
				typedef	StringT	string_type;
				typedef IteratorT	iterator;			
				
			public:
				Text() : XML::Text<StringT, IteratorT>::Text() {}
				Text(IteratorT it)
					 : XML::Text<StringT, IteratorT>::Text(it) {}
				Text(IteratorT begin, IteratorT end)
					 : XML::Text<StringT, IteratorT>::Text(begin, end) {}
				Text(StringT content) {
					XML::Text<StringT, IteratorT>::Text(content);
				}				
				virtual ~Text() {}
				
			protected:						
				virtual bool text_stop(IteratorT it) {
					if (XML::Text<StringT, IteratorT>::text_stop(it)) {
						IteratorT next = ++it;
						skip_whitespace(next);
						if (this->parent() && this->parent()->type() == XML::ELEMENT) {
							XML::Element<StringT, IteratorT>* parent = reinterpret_cast<XML::Element<StringT, IteratorT>* >(this->parent());
							if ( parent->name() == StringT("SCRIPT") && *next != '/')								//&& !(dynamic_cast<XML::Element<StringT, IteratorT>* >(this->parent())->name() == StringT("SCRIPT")))
								return false;
						}
						return true;
					}
					return false;
				}
		};
					
		template <typename StringT = icstring, typename IteratorT = typename StringT::iterator>
		class HtmlKeyword: public XML::XmlKeyword<StringT, IteratorT> 
		{
			public:
				typedef	StringT	string_type;
				typedef IteratorT	iterator;	
				
			public:
				HtmlKeyword() : XML::XmlKeyword<StringT, IteratorT>::XmlKeyword() {
					//init();
				}
				HtmlKeyword(IteratorT it) : XML::XmlKeyword<StringT, IteratorT>::XmlKeyword(it){
					//init();
				}
				HtmlKeyword(IteratorT begin, IteratorT end) : XML::XmlKeyword<StringT, IteratorT>::XmlKeyword(begin, end){
					//init();
				}
				HtmlKeyword(StringT content) {
					XML::XmlKeyword<StringT, IteratorT>::XmlKeyword(content);
					//init();
				}				
				virtual ~HtmlKeyword() {}				
				
			protected:
			public:
				virtual bool match(IteratorT begin, IteratorT end) {
					while (!XML::XmlKeyword<StringT, IteratorT>::is_start_symbol(begin))
						++begin;
					return XML::XmlKeyword<StringT, IteratorT>::match(begin, end);
				}
		};
		
		template <typename StringT = std::string, typename IteratorT = typename StringT::iterator>
		class Attribute : public XML::XmlAttribute<StringT, IteratorT> {			
			public:
				typedef std::map<StringT, Attribute*, ignorcase_string_compare>	attributes_type;
							
			public:
				Attribute(): 
					XML::XmlAttribute<StringT, IteratorT>::XmlAttribute() { init(); }
				Attribute(IteratorT begin, IteratorT end) : 
					XML::XmlAttribute<StringT, IteratorT>::XmlAttribute(begin, end) { init(); }
				virtual ~Attribute() {}		
				
			private:
				void init() { this->force_end_quote_ = false; }
											
		};
				
		template <typename StringT = std::string,
			 typename IteratorT = typename StringT::iterator,
			 typename AttributeT = Attribute<StringT, IteratorT> >
		class ElemTag: public XML::ElemTag<StringT, IteratorT, AttributeT > 
		{
			public:
				ElemTag() : 
					XML::ElemTag<StringT, IteratorT, AttributeT >::ElemTag() {}
				ElemTag(IteratorT it) : 
					XML::ElemTag<StringT, IteratorT, AttributeT >::ElemTag(it) {
				}
				ElemTag(IteratorT begin, IteratorT end) : 
					XML::ElemTag<StringT, IteratorT, AttributeT >::ElemTag(begin, end){}
				ElemTag(StringT content) {
					XML::ElemTag<StringT, IteratorT, AttributeT >::ElemTag(content);
				}				
				virtual ~ElemTag() { };
				
				virtual bool required_end_tag() {
					if (forbidden_end_tag())
						return false;
						
					if (is_optional_tag(this))
						return false;
					return true;
				}
				
				virtual bool forbidden_end_tag() {
					StringT name(this->name().begin(), this->name().end());
					if ( 
						name == StringT("INPUT")
						|| name == StringT("COL")
						|| name == StringT("LINK")
						|| name == StringT("BASE")
						|| name == StringT("IMG")
						|| name == StringT("PARAM")
						|| name == StringT("AREA")		
						|| name == StringT("HR")
						|| name == StringT("BR")
						|| name == StringT("BASEFONT")
						|| name == StringT("FRAME")
						|| name == StringT("ISINDEX")
						|| name == StringT("META")	
						)					
						return true;
					return false;
				}
				
				virtual bool make_following_element_as_child(ElemTag* next_tag_ptr) {
					StringT name(this->name().begin(), this->name().end());
					if ( name == StringT("HTML")
							|| name== StringT("BODY")
							|| name== StringT("HEAD")
							|| name== StringT("P")							
							)
						return true;
						
					//StringT next_tag_name(next_tag_ptr->begin(), next_tag_ptr->end());						
					//if ( (name == StringT("COLGROUP") && next_tag_name == StringT("COL"))
					//		|| (name== StringT("TBODY") && next_tag_name == StringT("TR"))
					//		|| (name== StringT("THEAD") && next_tag_name == StringT("TR"))
					//		|| (name== StringT("TFOOT") && next_tag_name == StringT("TR"))	
							//|| (name== StringT("SELECT") && next_tag_name == StringT("OPTION"))
							//|| (name== StringT("OPTGROUP") && next_tag_name == StringT("OPTION"))
							//|| (name== StringT("UL") && next_tag_name == StringT("LI"))
							//|| (name== StringT("OL") && next_tag_name == StringT("LI"))							
					//		)
					//	return true;
						
					// if both tags are from the same group, that means 
					if ( !(
							(is_optional_dl_group(this) && is_optional_dl_group(next_tag_ptr))
						|| (is_optional_table_group(this) && is_optional_table_group(next_tag_ptr))
						|| (is_optional_others_group(this) && is_optional_others_group(next_tag_ptr))
						)
							)
						return true;						
						
					// TODO may append other judgements here
					// like LI tag only end when meeting another LI or UL tag
					return false;
				}
				
				virtual bool force_close(ElemTag* next_tag_ptr) {
					StringT name(this->name().begin(), this->name().end());
					StringT last_tag_name(next_tag_ptr->name().begin(), next_tag_ptr->name().end());
					if ( (name == StringT("HEAD") && last_tag_name == StringT("BODY"))
							|| (name== StringT("BODY") && last_tag_name == StringT("HEAD"))
							)
						return true;
					return false;					
				}
				
				static bool is_optional_tag(ElemTag* tag_ptr) {
					if (is_optional_dl_group(tag_ptr))
						return true;
					else if (is_optional_table_group(tag_ptr))
						return true;
					else if (is_optional_others_group(tag_ptr))
						return true;
					/*
					StringT name(tag_ptr->begin(), tag_ptr->end());
					if ( name == StringT("LI") 
						|| name == StringT("DT")
						|| name == StringT("DD")
						|| name == StringT("THEAD")
						|| name == StringT("TFOOT")
						|| name == StringT("TBODY")
						|| name == StringT("COLGROUP")
						|| name == StringT("TR")
						|| name == StringT("TH")
						|| name == StringT("TD")
						|| name == StringT("OPTION")
						//|| name == StringT("HTML")
						//|| name == StringT("HEAD")
						//|| name == StringT("BODY")
						|| name == StringT("P")
						//|| name == StringT("")
						//|| name == StringT("")						
						)
						return true;
						*/
					return false;					
				}
				
			private:
				static bool is_optional_dl_group(ElemTag* tag_ptr) {
					StringT name(tag_ptr->begin(), tag_ptr->end());
					if ( name == StringT("DT")
						|| name == StringT("DD")
						//|| name == StringT("HTML")
						//|| name == StringT("HEAD")
						//|| name == StringT("BODY")
						//|| name == StringT("")
						//|| name == StringT("")						
						)
						return true;
					return false;					
				}

				static bool is_optional_table_group(ElemTag* tag_ptr) {
					StringT name(tag_ptr->begin(), tag_ptr->end());
					if ( name == StringT("THEAD")
						|| name == StringT("TFOOT")
						|| name == StringT("TBODY")
						|| name == StringT("TR")
						|| name == StringT("TH")
						|| name == StringT("TD")	
						|| name == StringT("COLGROUP")
						)
						return true;
					return false;					
				}
				
				static bool is_optional_others_group(ElemTag* tag_ptr) {
					StringT name(tag_ptr->begin(), tag_ptr->end());
					if ( name == StringT("LI") 
						|| name == StringT("OPTION")
						//|| name == StringT("P")					
						)
						return true;
					return false;					
				}				
		};		
		
		template <typename StringT = icstring
			, typename IteratorT = typename StringT::iterator>
		class HtmlNodeTypes : public XML::XmlNodeTypes<StringT, IteratorT> {
			public:
				typedef HtmlKeyword<StringT, IteratorT>	keyword_type;
				typedef ElemTag<StringT, IteratorT> tag_type;
				typedef Text<StringT, IteratorT> text_type;
		};
		
		template <typename StringT = icstring
			, typename IteratorT = typename StringT::iterator
			, typename NodeTypesT = HtmlNodeTypes<StringT, IteratorT>
			/*, int N = -1*/
			>
		class HtmlElement: public XML::Element<StringT, IteratorT, NodeTypesT> 
		{
			private:
				typedef typename NodeTypesT::text_type 		TextT;
				typedef typename NodeTypesT::tag_type		ElemTagT;
				typedef typename NodeTypesT::keyword_type	HtmlKeywordT;
				typedef XML::Element<StringT, IteratorT, NodeTypesT> XmlElement;
			
			public:			
				typedef	StringT	string_type;
				typedef IteratorT	iterator;
				typedef	typename NodeTypesT::basic_entity	 basic_entity;		
				
			private:
				//bool is_root_; 
				//int	 level_;
				
			public:
				static const StringT HTML_START_LABEL;
				 
			private:
				void init() {
					//if (N == 0) 
					//	is_root_ = true;
					//else
					//	is_root_ = false;
						
					//level_ = N;
				}
				
			public:
				HtmlElement() : 
					XML::Element<StringT, IteratorT, NodeTypesT>::Element() {}
				HtmlElement(IteratorT it) : 
					XML::Element<StringT, IteratorT, NodeTypesT>::Element(it) {
					init();
				}
				HtmlElement(IteratorT begin, IteratorT end) : 
					 XML::Element<StringT, IteratorT, NodeTypesT>::Element(begin, end) { 
					init();
				}
				HtmlElement(StringT content) : 
					XML::Element<StringT, IteratorT, NodeTypesT>::Element(content) {
					init();
				}				
				virtual ~HtmlElement() {};				
			
			protected:
				virtual bool is_start(IteratorT& it) {
					this->type_ = XML::ELEMENT;
					if (this->last_tag_ptr_) {
						this->start_k_ = this->last_tag_ptr_;
						this->last_tag_ptr_ = NULL;
						it = this->start_k_->end();
						this->begin(this->start_k_->begin());
						return true;
					}
					this->begin(it);
					return true;
				}
				
				//virtual IteratorT skip_not_valid_char(IteratorT& next) {
				//	return skip_whitespace(next);
				//}
				
			public:
				virtual bool match(IteratorT begin, IteratorT end) {
					if (!this->parent()) {
						IteratorT doc_end = end;
						IteratorT temp_begin;
							
						skip_whitespace(begin);
						if (!is_start(begin))
							return false;
												
						IteratorT temp_end = begin;
						this->begin(temp_end);
						this->end(end);
						match_text(temp_end);
						begin = temp_end;
						// TODO if there is any text instead of a <html> tag
						// I take it that all the rest are the children of root element -- <html>
						// if ()
						skip_not_valid_char(temp_end);					
						
						if (!this->last_tag_ptr_ || this->last_tag_ptr_->length() <= 0 )
							return false;
					
						StringT last_tag_name(this->last_tag_ptr_->name().begin(), this->last_tag_ptr_->name().end());
						StringT html_tag_name(HTML_START_LABEL.begin() + 1, HTML_START_LABEL.end() - 1);
						if (last_tag_name != html_tag_name) {
							//int count = 0;
							do {
								temp_begin = temp_end;
								temp_end = doc_end;
								XmlElement* child_ptr = new XmlElement(temp_begin, temp_end);								
								child_ptr->set_parent(reinterpret_cast<basic_entity*>(this));
								if (this->last_tag_ptr_) {
									child_ptr->set_start_keyword(this->last_tag_ptr_);
									this->last_tag_ptr_ = NULL;
								}
								if (!((child_ptr->match(temp_begin, temp_end)) 
										&& child_ptr->length() > 0)) {
									delete child_ptr;
									break;
								}
								temp_end = child_ptr->end();
								this->add(reinterpret_cast<basic_entity*>(child_ptr));
								match_text(temp_end);
								skip_not_valid_char(temp_end);	
								end = temp_end;
								//count++;
							} while (temp_end != doc_end);
							if (this->size() > 0)
								return true;
							return false;
						}
					} 
					return StringBound<StringT, IteratorT>::match(begin, end);
				}				
		};		
		
		template <typename StringT
			, typename IteratorT
			, typename NodeTypesT
			/*, int N*/
			>	
		const StringT HtmlElement<StringT, IteratorT, NodeTypesT>::HTML_START_LABEL("<HTML>");
	}
}		

//#include "stpl_html_entity.tcc"
#endif /*STPL_HTML_ENTITY_H_*/
