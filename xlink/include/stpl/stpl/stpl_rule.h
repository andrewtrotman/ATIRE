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

#ifndef STPL_RULE_H_
#define STPL_RULE_H_

#include "stpl_entity.h"
#include "stpl_scanner.h"
#include "stpl_typetraits.h"

namespace stpl {

	/*
	 * TODO make the mode into the rule
	 * EXTRACT - extract only the valid entity
	 * FULL - the whole input should be valid entities
	 */
	enum Mode {EXTRACT, FULL};

	template < typename EntityT
				, typename ContainerT
				, typename ScannerT =  Scanner<EntityT>
						>
	class BaseRule
	{
		protected:
			ContainerT* containter_ptr_;

		public:
			BaseRule(ContainerT* container_ptr) :
								 containter_ptr_(container_ptr) {}
			virtual ~BaseRule() {};

			virtual bool apply(ScannerT* scanner_ptr) = 0;
	};

	template < typename EntityT,
						typename ContainerT,
						typename ScannerT =  Scanner<EntityT>,
						typename SecondEntityT = EntityT
						>
	class Rule : public BaseRule<EntityT, ContainerT, ScannerT>
	{
	    typedef BaseRule<EntityT, ContainerT, ScannerT>	BaseRuleT;
	    typedef typename EntityT::iterator	IteratorT;

		protected:
			//ContainerT* containter_ptr_;
			Entity<BaseRuleT>	sub_rules_;
			bool to_continue_;
			//Scanner<EntityT>* scanner_ptr_;

		private:
			void init() {
				to_continue_ = false;
			}

		public:
			Rule(ContainerT* container_ptr/*, ScannerType* scanner_ptr*/) :
				BaseRule<EntityT, ContainerT, ScannerT>::BaseRule(container_ptr)/*,
								 containter_ptr_(), scanner_ptr_(scanner_ptr) */{ init(); }
			virtual ~Rule() {	}

			void set_continue(bool b) { to_continue_ = b; }

			void add_rule(BaseRuleT* sub_rule_ptr) {
				 sub_rules_.add(sub_rule_ptr);
			}

			virtual bool apply(ScannerT* scanner_ptr) {
				bool ret = false;
				if (sub_rules_.size() > 0) {
					sub_rules_.reset();
					while (sub_rules_.more())  {
						if (!(ret = (*sub_rules_.next())->apply(scanner_ptr)) && (!to_continue_))
								return ret;
					}
				}
				else {
					if (this->containter_ptr_) {
						SecondEntityT* entity_ptr = new SecondEntityT(scanner_ptr->current(), scanner_ptr->end());
						EntityT* base_ptr = reinterpret_cast<EntityT*>(entity_ptr);
						ret = scanner_ptr->scan(base_ptr);
						if (ret ) {
							if (entity_ptr->length() > 0)  {
								this->containter_ptr_->add(base_ptr);
							}
							else {
								ret = false;
							}
						}

						if (!ret)
							delete entity_ptr;
					}
				}
				return ret;
			}
	};


	// 0...* entities, that mean could have any number of the entities


	template < typename EntityT,
						typename ContainerT,
						typename ScannerT =  Scanner<EntityT>,
						int N = -1,
						typename SecondEntityT = EntityT
						>
	class NRule : public Rule<EntityT, ContainerT, ScannerT, SecondEntityT>
	{
		public:
			//NRule() {}
			NRule(ContainerT* container_ptr) : Rule<EntityT, ContainerT, ScannerT, SecondEntityT>::Rule(container_ptr) {}
			virtual ~NRule() {	}

			virtual bool apply(ScannerT* scanner_ptr) {
				if (N > 0) {
					for (int i=0; i<N; i++) {
						if (!Rule<EntityT, ContainerT, ScannerT, SecondEntityT>::apply(scanner_ptr))
							return false;
					}
					return true;
				}
				bool ret = false;
				while((ret = Rule<EntityT, ContainerT, ScannerT, SecondEntityT>::apply(scanner_ptr))) {
					if (!ret)
						break;
				}
				return ret;
			}
	};

	/// 0 or 1 entity, that mean one or nothing
	template < typename EntityT,
						typename ContainerT,
						typename ScannerT =  Scanner<EntityT>
						>
	class OptionalRule : public Rule<EntityT, ContainerT, ScannerT>
	{
		public:
			OptionalRule(ContainerT* container_ptr) : Rule<EntityT, ContainerT, ScannerT>::Rule(container_ptr) {}
			virtual ~OptionalRule() {	}

			virtual bool apply(ScannerT* scanner_ptr) {
				Rule<EntityT, ContainerT, ScannerT>::apply(scanner_ptr);
				return true;
			}
	};

	template < typename BaseEntityT,
				typename ContainerT,
				typename ScannerT =  Scanner<BaseEntityT>,
				typename EntityT_A = BaseEntityT,
				typename EntityT_B = BaseEntityT
				>
	class OrRule : public BaseRule<BaseEntityT, ContainerT, ScannerT>
	{
		private:
			typedef Rule<BaseEntityT, ContainerT, ScannerT, EntityT_A> RuleA;
			typedef Rule<BaseEntityT, ContainerT, ScannerT, EntityT_B> RuleB;

		public:
			OrRule(ContainerT* container_ptr) :
				BaseRule<BaseEntityT, ContainerT, ScannerT>::BaseRule(container_ptr) {}
			virtual ~OrRule() {	}

			virtual bool apply(ScannerT* scanner_ptr) {
				EntityT_A a;
				EntityT_B b;

				while(!scanner_ptr->is_end()) {
					if (a.is_valid_char(scanner_ptr->current())) {
						RuleA rule_a(this->containter_ptr_);
						return rule_a.apply(scanner_ptr);
					} else if (b.is_valid_char(scanner_ptr->current())) {
						RuleB rule_b(this->containter_ptr_);
						return rule_b.apply(scanner_ptr);
					}
					scanner_ptr->skip();
				}
				return false;
			}
	};

	template < typename BaseEntityT,
				typename ContainerT,
				typename ScannerT =  Scanner<BaseEntityT>,
				typename EntityT_A = BaseEntityT,
				typename EntityT_B = BaseEntityT
				>
	class AndRule : public BaseRule<BaseEntityT, ContainerT, ScannerT>
	{
		public:
			AndRule(ContainerT* container_ptr) :
				BaseRule<BaseEntityT, ContainerT, ScannerT>::BaseRule(container_ptr) {}
			virtual ~AndRule() {	}

			virtual bool apply(ScannerT* scanner_ptr) {
				Rule<BaseEntityT, ContainerT, ScannerT, EntityT_A> rule_a(this->containter_ptr_);
				Rule<BaseEntityT, ContainerT, ScannerT, EntityT_B> rule_b(this->containter_ptr_);
				return rule_a.apply(scanner_ptr)
						&& rule_b.apply(scanner_ptr);
			}
	};

	template < typename EntityT,
						typename ContainerT,
						typename ScannerT =  Scanner<EntityT>
						>
	class SkipRule : public Rule<EntityT, ContainerT, ScannerT>
	{
		public:
			SkipRule(ContainerT* container_ptr) : Rule<EntityT, ContainerT, ScannerT>::Rule(container_ptr) {}
			virtual ~SkipRule() {	}

			virtual bool apply(ScannerT* scanner_ptr) {
				while(!scanner_ptr->is_end()) {
					//EntityT entity;
					if (EntityT().is_valid_char(scanner_ptr->current()))
						return Rule<EntityT, ContainerT, ScannerT>::apply(scanner_ptr);
					scanner_ptr->skip();
				}
				return false;
			}
	};
}


#endif /*STPL_RULE_H_*/
