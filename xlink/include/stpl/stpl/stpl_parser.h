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

#ifndef STPL_PARSER_H_
#define STPL_PARSER_H_

#include "stpl_grammar.h"
#include "stpl_scanner.h"
#include "stpl_doc.h"
#include "stpl_othertraits.h"

namespace stpl {

	template<
				typename GrammarT = GeneralGrammar<Document<> >,
				typename DocumentT = typename GrammarT::document_type,
	 			typename EntityT = typename DocumentT::entity_type,
				typename ScannerT = Scanner<EntityT>
			 >
	class Parser {
		private:
			typedef	typename EntityT::string_type 	StringT;
			typedef typename EntityT::iterator		IteratorT;

		public:
			typedef typename EntityT::string_type	 string_type;
			typedef typename EntityT::iterator		 iterator;
			typedef DocumentT						 document_type;

		private:
			bool		own_doc_;

		protected:
			DocumentT	*doc_;
			ScannerT	scanner_;
			GrammarT	grammar_;

		public:
			Parser(IteratorT begin, IteratorT end) :
							doc_(new DocumentT(begin, end)),
							scanner_(begin, end),
							grammar_(doc_)
			{
				own_doc_ = true;
			}

			Parser(DocumentT *doc) :
							doc_(doc),
							scanner_(doc->begin(), doc->end()),
							grammar_(doc_)
			{
				own_doc_ = false;
			}

			virtual ~Parser()
			{
				if (own_doc_) delete doc_;
			}

			virtual DocumentT& parse()
			{
				//scanner_.init(doc_.begin(), doc_.end());
				this->grammar_.reset();
				while (this->grammar_.more()) {
					 (*this->grammar_.next())->apply(&this->scanner_);
				}
				return doc();
			}

			virtual bool end()
			{
				return this->scanner_.is_end();
			}

			DocumentT& doc() { return *doc_; }
	};

	template<
				typename EntityT
			>
	class GeneralParser : public Parser<GeneralGrammar<Document<EntityT> > >
	{
	private:
		typedef	typename EntityT::string_type 	StringT;
		typedef typename EntityT::iterator		IteratorT;

		typedef Parser<GeneralGrammar<Document<EntityT> > >		Parent;
		typedef typename Parent::document_type 					DocumentT;

	private:
		bool first_time_;

	public:
		GeneralParser(IteratorT begin, IteratorT end) : Parent::Parser(begin, end)
		{
			init();
		}

		GeneralParser(DocumentT *doc) : Parent::Parser(doc)
		{
			init();
		}

		virtual ~GeneralParser()
		{
		}

		EntityT *get_next_entity()
		{
			if (first_time_) {
				this->doc().reset();
				first_time_ = false;
			}
			if (this->doc().more())
				return *(this->doc().next());
			return NULL;
		}

	private:
		void init()
		{
			first_time_ = true;
		}
	};
}

#endif /*STPL_PARSER_H_*/
