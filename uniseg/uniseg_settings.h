/*
 * UNISEQ_SETTINGS.H
 * -----------------
 *
 *  Created on: Jul 6, 2009
 *      Author: monfee
 */

#ifndef __UNISEQ_SETTINGS_H__
#define __UNISEQ_SETTINGS_H__

#include "uniseg_types.h"
#include <vector>

class UNISEG_settings
{
public:
	static const int MAX_CHARS = 100;
	static const char DEFAULT_FRQS_V_NAME[];
	static const char DEFAULT_DICS_V_NAME[];
	static const char DEFAULT_TDICS_V_NAME[];  // training dics, the dic with frequency and boundary information
	static const char DEFAULT_VAR[];
	static const char DEFAULT_CONF[];
	static const char DEFAULT_CONF_FILE[];
	static const char DEFAULT_INDEX[];
	static const char DEFAULT_ZH[];
	static const char DEFAULT_TR[];
	static const char DEFAULT_EN[];

public:
	bool 					load;
	bool 					verbose;
	bool					debug;

	std::vector<int> 		skip;
	int 					skip_high;
	int 					skip_low;
	int 					to_skip; // for all skipping
	bool					do_skip;

	int 					max;
	int 					min;

	long 					lang;

	std::string 			freqs_path; // string pattern frequency table path
	std::string 			dics_path;  // dictionary path
	std::string				training_path;

	std::string				conf_file;
	char  					sep[2];


	int 					mean;
	int 					mi;
	bool 					reward;
	bool					optimize;

	bool					split_latin_char;

	bool					stop_word_check;
	bool					oov_check;
	bool					on_training;
	bool					with_training_info;

	unsigned long 			encoding_scheme;		// which encoding scheme to be used for input text, currently UTF-8 and ASCII supported

	double					threshold;

	int						oov_threshold;
	long					focused_document_size;

private:
	void init();
	void init_fields();
	void init_others();

public:
	UNISEG_settings();
	~UNISEG_settings() {}

	static UNISEG_settings& instance();

	bool skipit(int size, int freq);
	void load_conf(const char *filename);
};
#endif /* __UNISEQ_SETTINGS_H__ */
