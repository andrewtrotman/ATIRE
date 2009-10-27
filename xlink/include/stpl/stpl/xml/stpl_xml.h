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

#ifndef STPL_XML_H_
#define STPL_XML_H_

//#include <fstream>

#include "stpl_xml_entity.h"
#include "../stpl_doc.h"
#include "../stpl_scanner.h"
#include "../stpl_grammar.h"
#include "../stpl_parser.h"

namespace stpl {
	namespace XML {

		template<	typename StringT = std::string,
					typename IteratorT = typename StringT::iterator,
					typename EntityT = BasicXmlEntity<StringT, IteratorT>,
					typename RootElemT = Element<
									typename EntityT::string_type,
									typename EntityT::iterator
									>  >
		class XDocument :  public Document<EntityT> {
			private:
				//typedef	typename EntityT::string_type StringT;
				//typedef typename EntityT::iterator	IteratorT;

			public:
				typedef EntityT										entity_type;
				typedef StringT										string_type;
				typedef IteratorT 									iterator;
				typedef typename RootElemT::tree_type				tree_type;
				typedef RootElemT 									element_type;
				typedef typename Document<EntityT>::entity_iterator entity_iterator;

			public:
				XDocument() : Document<EntityT>::Document() { init(); }
				XDocument(IteratorT it) : Document<EntityT>::Document(it) {
					 init();
				}
				XDocument(IteratorT begin, IteratorT end) :
					Document<EntityT>::Document(begin, end) {
					 init();
				}
				XDocument(StringT content) : Document<EntityT>::Document(content) {
					 init();
				}
				XDocument(StringT& content) : Document<EntityT>::Document(content) {
					 init();
				}
				virtual ~XDocument() {}

				RootElemT* root() { return root_; }
				void root(RootElemT* root) {
					root_ = root;
				}

				void write(std::string filename) {
					ofstream outfile (filename.c_str(),ofstream::binary);
					outfile << this->ref();
					outfile.close();
				}

				virtual void flush(int level=0) {
					//this->ref().erase();
					entity_iterator it;
					for (it = this->iter_begin(); it != this->iter_end(); ++it) {
						(*it)->flush(level);
						this->ref().append((*it)->ref());
					}
				}

			private:
				void init() {
					//debug
					// this->ref().append("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
				}

			private:
				RootElemT* 	root_;

		};

		typedef XDocument<>	XmlDocument;
		typedef XDocument<std::string, char*>	XmlFile;

		template <
			 	typename DocumentT = XmlDocument,
		 		typename EntityT = typename DocumentT::entity_type,
			 	typename ScannerT = Scanner<EntityT>,
			 	typename BaseRuleT = BaseRule<EntityT, DocumentT, ScannerT>
		 	 >
		class BasicXmlGrammar : public Grammar<DocumentT, EntityT, ScannerT, BaseRuleT> {
			public:
				typedef typename EntityT::string_type	 string_type;
				typedef typename EntityT::iterator		 iterator;

			private:
				typedef Rule<EntityT, DocumentT, ScannerT> RuleT;

				typedef Element<
								string_type,
								iterator,
								XmlNodeTypes<string_type, iterator>/*,
										0*/
								> TempElement;

				typedef InfoNode<
								string_type,
								iterator
								> TempInfoNode;

				typedef NRule<
								TempElement,
								DocumentT, Scanner<TempElement>,
								1
								>	OneRootElementRule;

				typedef NRule<
								TempInfoNode,
								DocumentT, Scanner<TempInfoNode>
								>	NInfoNodeRule;

			protected:
				void add_rules() {
					RuleT* rule_ptr = new RuleT(this->document_ptr_);
					rule_ptr->set_continue(true);
					rule_ptr->add_rule(reinterpret_cast<BaseRuleT*>(new NInfoNodeRule(this->document_ptr_)));
					rule_ptr->add_rule(reinterpret_cast<BaseRuleT*>(new OneRootElementRule(this->document_ptr_)));
					this->add(reinterpret_cast<BaseRuleT*>(rule_ptr));
				}

				void init() {
					add_rules();
				}

			public:
				BasicXmlGrammar(DocumentT*	document_ptr)  :
						Grammar<DocumentT, EntityT, ScannerT, BaseRuleT>::Grammar(document_ptr) { init(); }
				virtual ~BasicXmlGrammar() {}

		};

		template<
					typename StringT = std::string,
					typename IteratorT = typename StringT::iterator,
					typename DocumentT = XDocument<StringT, IteratorT>,
					typename GrammarT = BasicXmlGrammar<DocumentT>,
		 			typename EntityT = typename DocumentT::entity_type,
					typename ScannerT = Scanner<EntityT>
				 >
		class XParser : public Parser<
									GrammarT
									, DocumentT
									, EntityT
									, ScannerT
								>{
			public:
				typedef typename DocumentT::tree_type 		tree_type;
				typedef typename DocumentT::element_type 	element_type;
				typedef DocumentT							document_type;
				typedef typename EntityT::string_type	 	string_type;
				typedef typename EntityT::iterator		 	iterator;

			protected:
				tree_type	tree_;


			public:
				XParser(IteratorT begin, IteratorT end) : Parser<GrammarT
																, DocumentT
																, EntityT
																, ScannerT
																>::Parser(begin, end) { }
				virtual ~XParser() {}

				tree_type& parse_tree(StringT& content) {
					this->parse_tree(content.begin(), content.end());
				}

				tree_type& parse_tree(IteratorT begin, IteratorT end) {
					init(begin, end);
					return this->parse_tree();
				}

				tree_type& parse_tree() {
					parse();
					root()->traverse(tree_);
					return tree_;
				}

				virtual void parse() {
					Parser<GrammarT
							, DocumentT
							, EntityT
							, ScannerT
							>::parse();
					this->doc().reset();
					while (this->doc().more()) {
						typename DocumentT::entity_iterator it = this->doc().next();
						if ((*it)->type() == EntityT::element_type()) {
							this->doc().root(reinterpret_cast<element_type*>(*it));
							break;
						}
					}
				}

				element_type* root() { return this->doc().root(); }

		};

	}
}

#endif /*STPL_XML_H_*/
