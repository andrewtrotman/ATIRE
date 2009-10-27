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

#ifndef STPL_GRAMMAR_H_
#define STPL_GRAMMAR_H_

#include "stpl_rule.h"
#include "stpl_entity.h"
#include "stpl_doc.h"
#include "stpl_scanner.h"

namespace stpl {
	template <
	 	typename DocumentT = Document<>,
	 	typename EntityT = typename DocumentT::entity_type,
	 	typename ScannerT = Scanner<EntityT>,
	 	typename RuleT = Rule<EntityT, DocumentT, ScannerT>
	 	 >
	class Grammar : public Entity<RuleT>{
		public:
			typedef DocumentT	document_type;
			typedef typename EntityT::string_type	 string_type;
			typedef typename EntityT::iterator		 iterator;
		/*
		private:
			typedef Rule<EntityT, DocumentT, ScannerT> 			DefaultRuleT;
			typedef BaseRule<EntityT, DocumentT, ScannerT>		BaseRuleT;
			*/

		protected:
			DocumentT*		document_ptr_;

		public:
			Grammar(DocumentT*	document_ptr) : document_ptr_(document_ptr) {
			}
			virtual ~Grammar() {}

		private:
			virtual void init() = 0;
			/// the initialization function has to be implemented and call add_rules function
			///{
			///	add_rules();
			///}

			virtual void add_rules() = 0;
	};

	template <
		 	typename DocumentT = Document<>,
	 		typename EntityT = typename DocumentT::entity_type,
		 	typename ScannerT = Scanner<EntityT>,
		 	typename RuleT = NRule<EntityT, DocumentT, ScannerT>
	 	 >
	class GeneralGrammar : public Grammar<DocumentT, EntityT, ScannerT, RuleT> {
		private:
			virtual void add_rules() {
				this->add(new RuleT(this->document_ptr_));
			}

			void init() {
				add_rules();
			}

		public:
			GeneralGrammar(DocumentT*	document_ptr)  :
					Grammar<DocumentT, EntityT, ScannerT, RuleT>::Grammar(document_ptr) { init(); }
			virtual ~GeneralGrammar() {}

	};
}

#endif /*STPL_GRAMMAR_H_*/
