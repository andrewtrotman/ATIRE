/*
 * algorithm_page_name.cpp
 *
 *  Created on: Jan 26, 2010
 *      Author: monfee
 */

#include "algorithm_page_name.h"
#include "algorithm_config.h"
#include "ant_link_parts.h"
#include "ant_link_term.h"
#include "ant_link_posting.h"
#include "language.h"

#include <stdlib.h>

#include <fstream>
#include <utility>
#include <sstream>
#include <iostream>

using namespace std;

namespace QLINK
{

	algorithm_page_name::algorithm_page_name()
	{
		init();
	}

	algorithm_page_name::~algorithm_page_name()
	{
		page_map_t::iterator iter = names_map_.begin();
		for (; iter != names_map_.end(); ++iter) {
			if (iter->second)
				delete iter->second;
		}
	}

	void algorithm_page_name::init()
	{
		config_ = new algorithm_config;
		loaded_ = false;

		load_names();
	}

	void algorithm_page_name::load_names()
	{
		if (!config_ || loaded_)
			return;

		string filename(config_->get_value("titles_file"));
		if (filename.length() <= 0)
			return;

		ifstream myfile (filename.c_str());

		if (myfile.is_open()) {
			while (! myfile.eof()) {
				string line;
				getline (myfile, line);
				if (line.length() == 0 || line.find("Portal:") != string::npos)
					continue;

				string::size_type pos = line.find_first_of(':');
				if (pos != string::npos) {
					unsigned long doc_id = atol(line.c_str());
					string wiki_title(line, ++pos);
					std::pair<std::string, std::string> title_pair = wikipedia::process_title(wiki_title, lowercase_only);
					//struct wiki_entry a_entry;
					ANT_link_posting *a_entry = new ANT_link_posting;
					a_entry->docid = doc_id;
					a_entry->desc = strdup(title_pair.second.c_str());
					a_entry->offset = 0;
					a_entry->link_frequency = 1;
					a_entry->doc_link_frequency = 1;
					//a_entry.id = doc_id;
					//a_entry.title = title_pair.first;
					//a_entry.description = title_pair.second;

					page_map_t::iterator iter = names_map_.find(title_pair.first);
					if (iter == names_map_.end() || iter->second == NULL) {
						//wiki_entry_array wea;
						ANT_link_term *term = new ANT_link_term;
						term->term = strdup(title_pair.first.c_str());
						term->total_occurences = 0;
						term->document_frequency = 9999;
						term->collection_frequency = 9999;
						string& name = title_pair.first;

						if (iter != names_map_.end() && iter->second == NULL)
							iter->second = term;
						else
							iter = names_map_.insert(make_pair(name, term)).first;

						string::size_type pos = name.find(' ');
						if (pos != string::npos) {
							string first_term(name, 0, pos);
							page_map_t::iterator iner_iter = names_map_.find(first_term);
							if (iner_iter == names_map_.end())
								names_map_.insert(make_pair(first_term, (ANT_link_term *)NULL));
	//						else
	//							cerr << "found a match (" << iner_iter->first << ")" << endl;
						}
						//result.second.push_back(a_entry);
					}
					iter->second->postings.push_back(a_entry);
				}
				else {
					cerr << "Error in line : " << line << endl;
				}

			  //cout << line << endl;
			}
			myfile.close();
		}

		// debug
	//	page_map_t::iterator iter = names_map_.begin();
	//	while (iter != names_map_.end()) {
	//		if (iter->second != NULL)
	//			for (int i = 0; i < iter->second->postings.size(); i++)
	//				cerr << iter->second->postings[i]->docid << ":"
	//						<< iter->first << "(" << iter->second->postings[i]->desc << ")" << endl;
	//		else
	//			cerr << "found a match (" << iter->first << ") for the easy indexing" << endl;
	//		++iter;
	//	}

		loaded_ = true;
	}

	void algorithm_page_name::process_terms(links *lx, char **term_list, const char *source)
	{
		search_anchor_by_page_nam(lx, term_list, source);
		lx->sort_links_by_term();


	}

	void algorithm_page_name::process_terms(char **term_list, const char *source)
	{
		process_terms(links_, term_list, source);
	}

	void algorithm_page_name::search_anchor_by_page_nam(links* lx, char **term_list, const char *source)
	{
		char **first, **last;
		char *where_to;
		long offset = 0, term_len = 0;
		char buffer[1024 * 1024];
		page_map_t::iterator index_entry = names_map_.end();
		page_map_t::iterator last_index_entry = index_entry;
		ANT_link_term *node = NULL;

		for (first = term_list; *first != NULL; first++) {
			where_to = buffer;
			last_index_entry = names_map_.end();

			// debug
	//		cerr << *first << "$";
	//		if (strncmp(*first, "gorseinon", 9) == 0)
	//			cerr << " I caught you" << endl;

			int terms_count = 0;
			for (last = first; *last != NULL && terms_count < 13; last++) {
				terms_count++;

				if (where_to == buffer) {
					strcpy(buffer, *first);
					where_to = buffer + strlen(buffer);

					index_entry = names_map_.find(buffer);

					if (index_entry == names_map_.end())
						break;

					if (index_entry->second != 0)
						last_index_entry = index_entry;
				}
				else {
					*where_to++ = ' ';
					strcpy(where_to, *last);
					where_to += strlen(*last);


					// debug
	//				if (strcmp(buffer, "gorseinon bus station") == 0)
	//					cerr << " I caught you" << endl;

					string what(buffer);
					index_entry = names_map_.find(what);

					if (index_entry == names_map_.end())
						continue;									// we're after the last term in the list
								// we're a term in the list, but might be a longer one so keep looking

					last_index_entry = index_entry;
				}
	//			if (strncmp(buffer, index_term->term, strlen(buffer)) != 0)
	//				break;									// we can't be a substring so we're done
			}

			if (last_index_entry != names_map_.end()) {
				bool to_skip = false;
				if (stopword_no_)
					if (!strpbrk(last_index_entry->first.c_str(), "- "))
						to_skip = language::isstopword(last_index_entry->first.c_str());

				if (!to_skip) {
					offset = *first - source;
					term_len = strlen(last_index_entry->second->term);
					strncpy(buffer, offset + text_, term_len);
					buffer[term_len] = '\0';
					node = last_index_entry->second; //new ANT_link_term;
					//node->term = strdup(last_index_entry->first.c_str());
					fprintf(stderr, "%s -> %d ", last_index_entry->second->term, last_index_entry->second->postings[0]->docid);
					if (!lx->find(last_index_entry->second->term))
						link * lnk = lx->push_link(NULL, offset, last_index_entry->second->term, last_index_entry->second->postings[0]->docid, 0.0, node);
//					if (!lx->find(last_index_term->term))
//						link * lnk = lx->push_link(NULL, offset, last_index_term->term, last_index_entry->second->postings[0]->docid, 0.0, node);
					else
						fprintf(stderr, "Duplicated");
					fprintf(stderr, "\n");
					//lnk->require_cleanup();
					//create_posting(last_index_entry->second, lnk);
				}
			}
		}
		// debug
	//	cerr << endl;
	}

	//wiki_entry_array *algorithm_page_name::find_page(char *name)
	//{
	//	std::map<std::string, wiki_entry_array>::iterator iter = names_map_.find(name);
	//	return iter == names_map_.end() ? NULL : &(iter->second);
	//}

	//void algorithm_page_name::create_posting(wiki_entry_array& wea, link* lnk)
	//{
	//	//lnk->link_term->postings_length = wea.size();
	//
	//	lnk->link_term->total_occurences = 0;
	//	lnk->link_term->document_frequency = 9999;
	//	lnk->link_term->collection_frequency = 9999;
	//
	//	for (int i = 0; i < wea.size(); i++) {
	//		lnk->link_term->postings.push_back(new ANT_link_posting); //[wea.size()];
	//		lnk->link_term->postings[i]->docid = wea[i].id;
	//	}
	//}


}
