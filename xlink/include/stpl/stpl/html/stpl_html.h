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

#ifndef STPL_HTML_H_
#define STPL_HTML_H_

#include "../stpl_rule.h"
#include "stpl_html_entity.h"
#include "../stpl_parser.h"
#include "../xml/stpl_xml.h"
#include "../../utils/icstring.h"

namespace stpl {
	namespace HTML {
		//using namespace XML;

		//typedef Document<StringBound<> >	HtmlDocument;
		typedef Document<XML::BasicXmlEntity<icstring> >	HtmlDocument;
		typedef Document<XML::BasicXmlEntity<icstring, char*> >	HtmlFile;

		// includes HTML or XHTML
		template <
			 	typename DocumentT = HtmlDocument,
		 		typename EntityT = typename DocumentT::entity_type,
			 	typename ScannerT = Scanner<EntityT>,
			 	typename BaseRuleT = BaseRule<EntityT, DocumentT, ScannerT>
		 	 >
		class BasicHtmlGrammar : public Grammar<DocumentT, EntityT, ScannerT, BaseRuleT> {
			public:
				typedef typename EntityT::string_type	 	string_type;
				typedef typename EntityT::iterator		 	iterator;

			private:
				typedef Rule<EntityT, DocumentT, ScannerT> RuleT;

				typedef XML::InfoNode<	string_type,
										iterator
										> InfoNode;

				typedef OptionalRule<	InfoNode,
										DocumentT, Scanner<InfoNode>
										> OptionalInfoNodeRule;

				typedef XML::DocType<	string_type,
										iterator
										> DocType;

				typedef OptionalRule<	DocType,
										DocumentT, Scanner<DocType>
										> OptionalDocTypeRule;

				typedef HtmlElement<    string_type,
										iterator,
										HtmlNodeTypes<string_type, iterator>/*,
										0*/
										> HtmlElementType;

				typedef NRule<
								HtmlElementType,
								DocumentT, Scanner<HtmlElementType>,
								1
								>	OneElementRule;

			public:
				typedef HtmlElementType						element_type;

			protected:
				void add_rules() {
					RuleT* rule_ptr = new RuleT(this->document_ptr_);
					rule_ptr->add_rule(reinterpret_cast<BaseRuleT*>(new OptionalInfoNodeRule(this->document_ptr_)));
					rule_ptr->add_rule(reinterpret_cast<BaseRuleT*>(new OptionalDocTypeRule(this->document_ptr_)));
					rule_ptr->add_rule(reinterpret_cast<BaseRuleT*>(new OneElementRule(this->document_ptr_)));
					this->add(reinterpret_cast<BaseRuleT*>(rule_ptr));
				}

				void init() {
					add_rules();
				}

			public:
				BasicHtmlGrammar(DocumentT*	document_ptr)  :
						Grammar<DocumentT, EntityT, ScannerT, BaseRuleT>::Grammar(document_ptr) { init(); }
				virtual ~BasicHtmlGrammar() {}

		};

		typedef HtmlDocument::entity_type HtmlEntityType;
		typedef Scanner<HtmlDocument::entity_type> HtmlScanner;
		typedef BasicHtmlGrammar<HtmlDocument> HtmlGrammar;
		typedef HtmlGrammar::element_type	HtmlElementType;

		typedef Parser<HtmlGrammar
						, HtmlDocument
						, HtmlEntityType
						, HtmlScanner
						> HtmlParser;

		typedef HtmlFile::entity_type HtmlFileEntityType;
		typedef Scanner<HtmlFile::entity_type> HtmlFileScanner;
		typedef BasicHtmlGrammar<HtmlFile> HtmlFileGrammar;

		typedef HtmlFileGrammar::element_type HtmlFileElementType;

		typedef Parser<HtmlFileGrammar
						, HtmlFile
						, HtmlFileEntityType
						, HtmlFileScanner
						> HtmlFileParser;

		template<
					typename StringT = icstring,
					typename IteratorT = typename StringT::iterator,
					typename DocumentT = XML::XDocument<StringT, IteratorT,
										XML::BasicXmlEntity<StringT, IteratorT>,
										HtmlElement<StringT, IteratorT>
										>,
					typename GrammarT = BasicHtmlGrammar<DocumentT>,
		 			typename EntityT = typename DocumentT::entity_type,
					typename ScannerT = Scanner<EntityT>
				 >
		class HParser : public XML::XParser<
									StringT,
									IteratorT,
									DocumentT,
									GrammarT,
									EntityT,
									ScannerT
								> {
			public:
				typedef typename DocumentT::tree_type 		tree_type;
				typedef typename DocumentT::element_type 	element_type;
				typedef DocumentT							document_type;
				typedef typename EntityT::string_type	 	string_type;
				typedef typename EntityT::iterator		 	iterator;

			public:
				//HParser() : XML::XParser<
				//					StringT,
				//					IteratorT,
				//					DocumentT,
				//					GrammarT,
				//					EntityT,
				//					ScannerT
				//					>::XParser() {}
				HParser(IteratorT begin, IteratorT end) :
											XML::XParser<
													StringT,
													IteratorT,
													DocumentT,
													GrammarT,
													EntityT,
													ScannerT
													>::XParser(begin, end)
							 {}
				virtual ~HParser() {}
		};
	}
}

#endif /*STPL_HTML_H_*/
