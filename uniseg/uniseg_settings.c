/*
 * UNISEQ_SETTINGS.C
 * -----------------
 *
 *  Created on: Jul 6, 2009
 *      Author: monfee
 */

#include "uniseg_settings.h"
#include "config.h"

#include <stdlib.h>

#include <algorithm>
#include <string>

const char UNISEG_settings::DEFAULT_FRQS_V_NAME[] = { "UNISEG_FRQS" };
const char UNISEG_settings::DEFAULT_DICS_V_NAME[] = { "UNISEG_DICS" };
const char UNISEG_settings::DEFAULT_TDICS_V_NAME[] = { "UNISEG_TDICS" };
const char UNISEG_settings::DEFAULT_VAR[] = {"var"};
const char UNISEG_settings::DEFAULT_CONF[] = {"conf"};
const char UNISEG_settings::DEFAULT_CONF_FILE[] = {"freqs.conf"};
const char UNISEG_settings::DEFAULT_INDEX[] = {"index"};
const char UNISEG_settings::DEFAULT_ZH[] = {"zh"};
const char UNISEG_settings::DEFAULT_TR[] = {"tr"};
const char UNISEG_settings::DEFAULT_EN[] = {"en"};

UNISEG_settings::UNISEG_settings()
{
	init();
}

void UNISEG_settings::init()
{
	init_fields();
	init_others();
}

void UNISEG_settings::init_fields()
{
	#if defined(DEBUG) || defined(_DEBUG)
		debug = true;
	#else
		debug = false; //false;
	#endif

	verbose = true;
	load = false;

	to_skip = 1;
	skip_high = 0;
	skip_low = 0;
	do_skip = false;

	lang = uniseg_encoding::UNKNOWN;

	reward = false;
	mean = 5;
	mi = 2;

	max = 5;

	#ifdef _MSC_VER
		sep[0] = '\\';
	#else
		sep[0] = '/';
	#endif

	sep[1] = '\0';

	split_latin_char = false;

	encoding_scheme = uniseg_encoding_factory::UTF8;

	stop_word_check = false;
	oov_check = false;

	threshold = 0.0;
	on_training = false;

	oov_threshold = 5;
	focused_document_size = 0;
}

void UNISEG_settings::init_others()
{
	if (getenv(DEFAULT_FRQS_V_NAME))
		freqs_path = std::string(getenv(DEFAULT_FRQS_V_NAME)) + std::string(sep);
	if (getenv(DEFAULT_DICS_V_NAME))
		dics_path = std::string(getenv(DEFAULT_DICS_V_NAME));
	if (getenv(DEFAULT_TDICS_V_NAME))
		training_path = std::string(getenv(DEFAULT_TDICS_V_NAME)) + std::string(sep);
}

UNISEG_settings& UNISEG_settings::instance()
{
	static UNISEG_settings inst;
	return inst;
}

bool UNISEG_settings::skipit(int size, int freq)
{
	if (skip.size() <= 0)
		return false;

	std::vector<int>::iterator ret = std::find(skip.begin(), skip.end(), size);
	if (ret != skip.end()) {
		if (skip_low == 0 && skip_high != 0) {
			if (freq > skip_high)
				return true;
		}
		else if (skip_high == 0 && skip_low != 0) {
			if (freq < skip_low)
				return true;
		}
		else {
			if (freq > skip_low && freq < skip_high)
				return true;
		}
	}
	return false;
}

void UNISEG_settings::load_conf(const char *filename)
{
	Config conf;
	if (conf.load(filename)) {
		std::string temp = conf.get_property("freq_dir");
		if (temp.length() > 0)
			freqs_path = temp + std::string(sep);
		temp = conf.get_property("training_dir");
		if (temp.length() > 0)
			training_path = temp + std::string(sep);
	}
}
