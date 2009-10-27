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
 * $Revision:  $
 * $Date:  $
 *
 *******************************************************************************/

#ifndef STPL_SIMPLE_H_
#define STPL_SIMPLE_H_

#include "stpl_doc.h"
#include "stpl_scanner.h"

namespace stpl {

	template<typename EntityT = StringBound<> >
	class SimpleDocument : public Document<EntityT> {
		private:
			typedef	typename EntityT::string_type StringT;
			typedef typename EntityT::iterator	IteratorT;

		public:
			typedef EntityT	entity_type;
			typedef StringT	string_type;
			typedef IteratorT iterator;
			typedef typename Document<EntityT>::entity_iterator entity_iterator;			

		public:
			//SimpleDocument() : Document<EntityT>::Document() { 
			//}
			//SimpleDocument(IteratorT it) : 
			//	Document<EntityT>::Document(it) {
			//}
			SimpleDocument(IteratorT begin, IteratorT end) :
				Document<EntityT>::Document(begin, end), scanner_(begin, end) {
			}
			SimpleDocument(StringT content) :
				Document<EntityT>::Document(content), scanner_(this->begin(), this->end()) {
			}	
			
			virtual ~SimpleDocument() {}
			
			void parse() {
				while (1) {
					EntityT* entity_ptr = new EntityT();				
					if (scanner_.next(entity_ptr)) {
						this->add(entity_ptr);
					}
					else {
						delete entity_ptr;
						break;
					}
				}
			}
			
			void bound(IteratorT begin, IteratorT end) {
				StringBound<StringT, IteratorT>::bound(begin, end);
				scanner_.init(begin, end);
			}
			
		private:
			Scanner<EntityT>	scanner_;
	};
}
#endif /*STPL_SIMPLE_H_*/
