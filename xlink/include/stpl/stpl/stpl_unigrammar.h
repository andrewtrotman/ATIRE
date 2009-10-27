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

#ifndef STPL_UNIGRAMMAR_H_
#define STPL_UNIGRAMMAR_H_

#include "stpl_grammar.h"
#include "stpl_uniword.h"
#include "stpl_unichar.h"
#include "stpl_parser.h"


namespace stpl {

	namespace UNICODE {

		/**
		 * Grammar for plain text document in unicode encoding
		 */
		template <
			 	typename DocumentT,
		 		typename EntityT = typename DocumentT::entity_type,
			 	typename ScannerT = Scanner<EntityT>,
			 	typename BaseRuleT = BaseRule<EntityT, DocumentT, ScannerT>
		 	 >
		class ChineseGrammar : public Grammar<DocumentT, EntityT, ScannerT, BaseRuleT> {
			public:
				typedef typename EntityT::string_type	 string_type;
				typedef typename EntityT::iterator		 iterator;

			private:
				typedef NRule<EntityT, DocumentT, ScannerT> RuleT;

				typedef AlnumWord<
								string_type,
								iterator
								> AlnumW;

				typedef UNICODE::ChineseChar<
								string_type,
								iterator
								> ChineseC;

				typedef OrRule<
								EntityT,
								DocumentT,
								Scanner<EntityT>,
								AlnumW,
								ChineseC
								>	AlnumWordOrChineseCharRule;

			private:
				virtual	void add_rules() {
					RuleT* rule_ptr = new RuleT(this->document_ptr_);
					rule_ptr->add_rule(reinterpret_cast<BaseRuleT*>(new AlnumWordOrChineseCharRule(this->document_ptr_)));
					this->add(reinterpret_cast<BaseRuleT*>(rule_ptr));
				}

				void init() {
					add_rules();
				}

			public:
				ChineseGrammar(DocumentT*	document_ptr)  :
						Grammar<DocumentT, EntityT, ScannerT, BaseRuleT>::Grammar(document_ptr) { init(); }
				virtual ~ChineseGrammar() {}

		};

		template <
				typename CharT,
			 	typename DocumentT = Document<StringBound<typename CharT::string_type> >,
		 		typename EntityT = typename DocumentT::entity_type,
			 	typename ScannerT = Scanner<EntityT>,
			 	typename BaseRuleT = BaseRule<EntityT, DocumentT, ScannerT>
		 	 >
		class UniCharGrammar :
			public Grammar<DocumentT, EntityT, ScannerT, BaseRuleT> {
			public:
				typedef typename EntityT::string_type	 string_type;
				typedef typename EntityT::iterator		 iterator;

			private:
				typedef NRule<EntityT, DocumentT, ScannerT> RuleT;

				typedef NRule<
								CharT,
								DocumentT,
								Scanner<EntityT>
								> 							CharRule;

			private:
				virtual	void add_rules() {
					RuleT* rule_ptr = new RuleT(this->document_ptr_);
					rule_ptr->add_rule(reinterpret_cast<BaseRuleT*>(new CharRule(this->document_ptr_)));
					this->add(reinterpret_cast<BaseRuleT*>(rule_ptr));
				}

				void init() {
					add_rules();
				}

			public:
				UniCharGrammar(DocumentT*	document_ptr)  :
						Grammar<DocumentT, EntityT, ScannerT, BaseRuleT>::Grammar(document_ptr) { init(); }
				virtual ~UniCharGrammar() {}

		};

	}

}
#endif /*STPL_UNIGRAMMAR_H_*/
