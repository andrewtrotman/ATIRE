/*
 * UNISEG_PLUGIN.C
 *
 *  Created on: Jul 10, 2009
 *      Author: monfee
 */

#include "uniseg_plugin.h"
#include "seg.h"
#include "qfreq.h"
#include "encoding_factory.h"

#include <iostream>

using namespace std;

UNISEG_plugin::UNISEG_plugin() : uniseg_plugin_interface()
{
    //output_ = 0;
    count_ = 0;
    //seger = NULL;
    enc_ = UNISEG_encoding_factory::get_encoding();

    // load the string frequency table
    cout << "##################### initializing UNISEG segmentation module ######################" << endl;
    //seger.load_frqs();
    QFreq::instance().load_freq();
    cout << "############################# finished initialization ##############################" << endl;
}

UNISEG_plugin::~UNISEG_plugin()
{
    cleanup();
}

void UNISEG_plugin::cleanup()
{
    delete enc_;
}

const unsigned char *UNISEG_plugin::do_segmentation(unsigned char *c, int length)
{
	if (!c || strlen((char *)c) == 0 || length == 0)
		return NULL;

	/**
	 * segment a few characters a time
	 */
	unsigned char *current = c;
	unsigned char *next = current;
	unsigned char *end = c + length;
	const long step = QFreq::instance().count();
	long segmented_len = 0;
	long how_far = 0;
	long distance = 0;
	long count = 0;
	bool flag = false;

	while (current < end) {
	    while ((count < step) && next < end) {
		enc_->test_char(next);
		next += enc_->howmanybytes();
		count++;
	    }

	    flag = next < end;

	    how_far = next - current;
	    seger_.input(current, how_far);
	    seger_.start();
	    const array_type& words_list = seger_.best_words();
	    long i = 0;
	    long size = flag ? words_list.size() - 1 : words_list.size();
	    for (; i < size; i++) {
		string_type& word = words_list[i]->chars();
		output_.append(word + " ");
		segmented_len = word.length();
	    }

	    // put the last segment back to the remaining characters
	    current += segmented_len;
	    segmented_len = 0;
	}

	return get_output();
}

const unsigned char *UNISEG_plugin::get_input()
{
return (unsigned char *)input_.c_str();
}

const unsigned char *UNISEG_plugin::get_output()
{
return (unsigned char *)output_.c_str();
}

int UNISEG_plugin::get_count() { return count_; }


#ifdef  __cplusplus
extern "C" {
#endif
	uniseg_plugin_interface *maker() {
	   return new UNISEG_plugin;
	}

	class proxy
	{
	public:
	   proxy() {
		   std::cout << "Registering UNISEG to the plugin factory..." << std::endl;
		  // register the maker with the factory
		   plugin_factory[uniseg_plugin_interface::SEGMENTATION].maker = maker;
	   }
	};
	// our one instance of the proxy
	proxy p;
#ifdef  __cplusplus
}
#endif
