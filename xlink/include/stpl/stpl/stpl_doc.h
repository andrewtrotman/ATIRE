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

#ifndef STPL_DOC_H_
#define STPL_DOC_H_

#include "stpl_entity.h"
#include "stpl_typetraits.h"

namespace stpl {
	
	template<typename EntityT = StringBound<> >
	class Document :  public StringEntity<//typename EntityT::string_type, 
										//typename EntityT::iterator,
										EntityT> {
		private:											
			typedef	typename EntityT::string_type StringT;
			typedef typename EntityT::iterator	IteratorT;
		
		public:
			typedef EntityT	entity_type;
			typedef StringT	string_type;
			typedef IteratorT iterator;
			typedef typename StringEntity</*StringT, IteratorT, */EntityT>::entity_iterator entity_iterator;			
			
		protected:
			virtual void init(IteratorT begin, IteratorT end) {
				this->begin(begin);
				this->end(end);
			}
						
		public:
			Document() : StringEntity</*StringT, IteratorT, */EntityT>::StringEntity() { 
			}
			Document(IteratorT it) : 
				StringEntity</*StringT, IteratorT, */EntityT>::StringEntity(it) {
			}
			Document(IteratorT begin, IteratorT end) :
				StringEntity</*StringT, IteratorT, */EntityT>::StringEntity(begin, end) {
			}
			Document(StringT content) :
				StringEntity</*StringT, IteratorT, */EntityT>::StringEntity(content) {
			}
			
			virtual ~Document() {}
			
			int count() {
				return this->size();
			}
		
	};
}

#endif /*STPL_DOC_H_*/
