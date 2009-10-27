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

#ifndef STPL_TREC_DOC_H_
#define STPL_TREC_DOC_H_

#include "../xml/stpl_xml_entity.h"
#include "../stpl_doc.h"
#include "../stpl_scanner.h"
#include "../stpl_grammar.h"
#include "../stpl_parser.h"

namespace stpl {

	namespace TREC {
/*		template<
					typename EntityT = XML::Element<>
					  >
		class TrecDocument
			 	: public Document<EntityT> {
		private:
			typedef	typename EntityT::string_type StringT;
			typedef typename EntityT::iterator	IteratorT;

		public:
			typedef EntityT	entity_type;
			typedef StringT	string_type;
			typedef IteratorT Iterator;
			typedef typename Document< EntityT>::entity_iterator entity_iterator;

			public:
				TrecDocument(IteratorT it) : Document< EntityT>::Document(it) {
					 //this->init();
				}
				TrecDocument(IteratorT begin, IteratorT end) : Document<EntityT>::Document(begin, end) {
					 //this->init();
				}
				TrecDocument(StringT content) {
					Document<EntityT>::Document(content);
					 //this->init();
				}
				virtual ~TrecDocument() {}
		};*/

		template <
			 	typename DocumentT = Document<>,
		 		typename EntityT = typename DocumentT::entity_type,
			 	typename ScannerT = Scanner<EntityT>,
			 	typename RuleT = Rule<EntityT, DocumentT, ScannerT>/*NRule<EntityT, DocumentT, ScannerT>*/
		 	 >
		class TrecDocGrammar : public Grammar<DocumentT, EntityT, ScannerT, RuleT> {
		public:
			typedef typename EntityT::string_type	 				string_type;
			typedef typename EntityT::iterator		 				iterator;

		private:
			typedef XML::Element<
							string_type,
							iterator,
							XML::XmlNodeTypes<string_type, iterator>
							> 										XElement;

			typedef NRule<
							XElement,
							DocumentT, Scanner<XElement>
							>										NNodesRule;

		public:
			typedef XElement										node_type;

		private:
				void add_rules() {
					RuleT* rule_ptr = new RuleT(this->document_ptr_);
					rule_ptr->add_rule(reinterpret_cast<RuleT*>(new NNodesRule(this->document_ptr_)));
					this->add(reinterpret_cast<RuleT*>(rule_ptr));
				}

				void init() {
					add_rules();
				}

			public:
				TrecDocGrammar(DocumentT*	document_ptr)  :
						Grammar<DocumentT, EntityT, ScannerT, RuleT>::Grammar(document_ptr) { init(); }
				virtual ~TrecDocGrammar() {}

		};

	}
}
#endif /*STPL_TREC_DOC_H_*/
