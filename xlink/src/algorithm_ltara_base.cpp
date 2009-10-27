/*
 * algorithm_ltara_base.cpp
 *
 *  Created on: Oct 17, 2009
 *      Author: monfee
 */

#include "algorithm_ltara_base.h"
#include "algorithm_config.h"
#include "ant_link_parts.h"
#include "ant_link_term.h"
#include "ant_link_posting.h"
#include "string_utils.h"

#include <string>

using namespace QLINK;
using namespace std;

algorithm_ltara_base::algorithm_ltara_base(links *links_list) :
	algorithm_ant_link_this_and_page_name(links_list)
{
	init();
}

algorithm_ltara_base::algorithm_ltara_base()
{
	init();
}

algorithm_ltara_base::~algorithm_ltara_base()
{

}

void algorithm_ltara_base::init()
{
	load_names();
}

void algorithm_ltara_base::load_names()
{
	if (!config_)
		return;

	string filename(config_->get_value("teara_titles_file"));
	if (filename.length() <= 0)
		return;

	ifstream myfile (filename.c_str());

	if (myfile.is_open()) {
		while (! myfile.eof()) {
			string line;
			getline (myfile, line);
			if (line.length() == 0 || line.find("\\") != string::npos)
				continue;

			string::size_type pos = line.find_first_of(':');
			if (pos != string::npos) {
				string id_name(line, 0, pos);
				string teara_title(line, ++pos);
				std::vector<std::string> anchors;

				// deal with "and"
				string::size_type sep_pos = 0;
				string::size_type last_pos = 0;
				while ((sep_pos = teara_title.find(" and ", sep_pos)) != string::npos) {
					string anchor1(teara_title, last_pos, sep_pos);
					anchors.push_back(anchor1);
					last_pos = sep_pos + 5;
					sep_pos = last_pos;
				}
				string anchor2(teara_title, last_pos);
				anchors.push_back(anchor2);

				std::vector<std::string>::iterator it = anchors.begin();
				std::vector<std::string>::iterator insert_it;
				while (it != anchors.end()) {
					//insert_it = anchors.end();
					string tmp;
					string& anchor = *it;
					string::iterator str_it = anchor.begin();
//					while (str_it != anchor.end()) {
//						tmp.clear();

					// erase head spaces
					str_it = trim(anchor, str_it);

					while (str_it != anchor.end() && (*str_it == ' ' || isalnum(*str_it))) {
						tmp.push_back(tolower(*str_it));
						str_it++;
						//str_it = anchor.erase(str_it);
					}

					// erase trailing spaces
					//str_it = trim(anchor, str_it);

					if (str_it != anchor.end() && tmp.length() > 0) {
						anchor.erase(anchor.begin(), anchor.begin() + tmp.length());
						it = anchors.insert(it, tmp);
						//(it = insert_it)++;
					}

//						if (str_it == anchor.end())
//							break;
//						str_it++;
//					}
					//anchor.erase(str_pos, anchor.end());
					++it;
				}

				//std::pair<std::string, std::string> title_pair = wikipedia::process_title(wiki_title);
				//struct wiki_entry a_entry;
				//a_entry.id = doc_id;
				//a_entry.title = title_pair.first;
				//a_entry.description = title_pair.second;
				for (int i = 0; i < anchors.size(); i++) {
					string& name = anchors[i];

					trim_left(name, " ");
					trim_right(name, " ");
					std_tolower<std::string>(name);

					if (name.length() <= 0)
						continue;

					page_map_t::iterator iter = names_map_.find(anchors[i]);
					if (iter == names_map_.end() || iter->second == NULL) {
						//wiki_entry_array wea;
						ANT_link_term *term = new ANT_link_term;
						term->term = strdup(anchors[i].c_str());
						term->total_occurences = 1;
						term->document_frequency = 1;
						term->collection_frequency = 1;

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
					int j = 0;
					for (; j < iter->second->postings.size(); j++) {
						if (strcmp(iter->second->postings[j]->desc, id_name.c_str()) == 0)
							break;
					}
					if (j >= iter->second->postings.size()) {
						ANT_link_posting *a_entry = new ANT_link_posting;
						a_entry->docid = -1;
						a_entry->desc = strdup(id_name.c_str());
						a_entry->offset = 0;
						a_entry->link_frequency = 1;
						a_entry->doc_link_frequency = 1;
						iter->second->postings.push_back(a_entry);
					}
				}
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
//				cerr << iter->second->postings[i]->desc << ":\"" << iter->second->term << "\""<< endl;
////		else
////			cerr << "found a match (" << iter->first << ") for the easy indexing" << endl;
//		++iter;
//	}
}
