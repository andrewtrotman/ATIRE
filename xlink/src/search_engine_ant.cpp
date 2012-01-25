/*
 * search_engine_ant.cpp
 *
 *  Created on: Sep 30, 2009
 *      Author: monfee
 */

#include "search_engine_ant.h"
#include "sys_file.h"

#include <atire_api.h>

using namespace QLINK;

#define FALSE 0;

const char *search_engine_ant::ANT_CONFIG_FILE = "ant.conf";
const char *search_engine_ant::ANT_INDEX_FILE_PROPERTY = "index";
const char *search_engine_ant::ANT_DOCLIST_FILE_PROPERTY = "doclist";

search_engine_ant::search_engine_ant() : segmentation_(false)
{
	init();
}

search_engine_ant::search_engine_ant(const char *doclist_filename, const char *index_filename, bool segmentation)
	: index_filename_(index_filename), doclist_filename_(doclist_filename), segmentation_(segmentation)
{
	init();
}

search_engine_ant::~search_engine_ant()
{
//	ant_free(ant_);

	if (config_ant_)
		delete config_ant_;
}

void search_engine_ant::init()
{
	if (sys_file::exist(ANT_CONFIG_FILE))
		config_ant_ = new config(ANT_CONFIG_FILE);
	else
		config_ant_ = NULL;

	docids_ = NULL;

//	ant_ = ant_easy_init();
//
//	ant_params(ant_)->bm25_k1 = 0.7;
//	ant_params(ant_)->bm25_b = 0.3;
//
//	if (config_ant_ != NULL) {
//		if (config_ant_->get_value(ANT_DOCLIST_FILE_PROPERTY).length() > 0)
//			doclist_filename_ = config_ant_->get_value(ANT_DOCLIST_FILE_PROPERTY);
//		if (config_ant_->get_value(ANT_INDEX_FILE_PROPERTY).length() > 0)
//			index_filename_ = config_ant_->get_value(ANT_INDEX_FILE_PROPERTY);
//
//		double b = atof(config_ant_->get_value("bm25_b").c_str());
//		double k1 = atof(config_ant_->get_value("bm25_k1").c_str());
//		if (b > 0)
//			ant_params(ant_)->bm25_b = b;
//		if (k1 > 0)
//			ant_params(ant_)->bm25_k1 = k1;
//
//		string rf_str = config_ant_->get_value("ranking_function");
//		if (rf_str == "BM25")
//			ant_params(ant_)->ranking_function = BM25;
//		else if (rf_str == "BOSE_EINSTEIN")
//			ant_params(ant_)->ranking_function = BOSE_EINSTEIN;
//		else if (rf_str == "DIVERGENCE")
//			ant_params(ant_)->ranking_function = DIVERGENCE;
//		else
//			ant_params(ant_)->ranking_function = BM25;
//	}
//
//	ant_params(ant_)->logo = FALSE;
//	ant_params(ant_)->stats = NONE;
//	ant_params(ant_)->output = stderr;
//	ant_params(ant_)->results_list_length = 10;
//	if (doclist_filename_.length() > 0)
//		ant_params(ant_)->doclist_filename = doclist_filename_.c_str();
//	if (index_filename_.length() > 0)
//		ant_params(ant_)->index_filename = index_filename_.c_str();
////	ant_params(ant_)->ranking_function = LMJM;
//	ant_params(ant_)->segmentation = segmentation_;
//
//	ant_setup(ant_);
}

const char **search_engine_ant::search(const char * term)
{
//	docids_ = (const char **)ant_search(ant_, &hits_, (char *)term);
	return docids_;
}
