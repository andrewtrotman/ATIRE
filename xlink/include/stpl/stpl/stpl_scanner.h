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

#ifndef STPL_SCANNER_H
#define STPL_SCANNER_H

#include "stpl_entity.h"
#include "stpl_doc.h"
#include "stpl_typetraits.h"
	
namespace stpl {
	
	template<	typename EntityT  >
	class Scanner{
		private:										
			typedef	typename EntityT::string_type StringT;
			typedef typename EntityT::iterator	IteratorT;
			
		public:			
			typedef	StringT string_type;
			typedef IteratorT	iterator;			
					
	 	public:
	 		Scanner() {}
	 		Scanner(IteratorT begin, IteratorT end)
	 		/*: currentState_(begin),  end_(end), begin_(begin)*/ {
	 			set(begin, end);
	 		}
	 		~Scanner() {
	 			clear_last();
	 		}
	 		
	 		bool is_end() {
				return (currentState_ == end_);	 			
	 		}
	 		
	 		EntityT* next() {
	 			clear_last();
	 			last_e_ = new EntityT();
	 			if (scan(last_e_))
	 				return last_e_;
	 			return NULL;
	 		}
	 		
	 		bool next(EntityT* entity) {
	 			return scan(entity);
	 		}	 		
	 		
	 		bool scan(EntityT* entity_ptr) {
	 				if (this->is_end())
	 					return false;
	 					
					IteratorT	begin = this->current();
					IteratorT	end = this->end();	 	
					/*
					 * TODO make the skipping of the non-valid char happen here 
					 */
					bool ret = entity_ptr->match(begin, end);
					if (ret)		
						currentState_ = entity_ptr->end();
					
					return ret;
	 		}	 		
	 		
	 		IteratorT current() { return currentState_; }
	 		IteratorT end() { return end_; }
	 		
			void set(IteratorT begin, IteratorT end) {
				begin_ = currentState_ = begin;					
				end_ = end;	
				
				last_e_ = NULL;
			}
			
			void skip() {
				if (!is_end())
					currentState_++;
			}
				 		
	 	private:
	 		void clear_last() {
	 			if (last_e_) {
	 				delete last_e_;
	 				last_e_ = NULL;
	 			}	 			
	 		}
			
	 	private:
	 		IteratorT 	currentState_;
	 		IteratorT 	end_;
	 		IteratorT 	begin_; 			
	 		
	 		EntityT*	last_e_;
	};
}

//#include "stpl_scanner.tcc"

#endif // STPL_SCANNER_H
