/*
 * UNISEQ_SETTINGS.H
 * -----------------
 *
 *  Created on: Jul 6, 2009
 *      Author: monfee
 */

#ifndef __UNISEQ_SETTINGS_H__
#define __UNISEQ_SETTINGS_H__

class LIB_uniseg_settings
{
public:
	static const int MAX_CHARS = 100;
	enum Format {XML, TEXT, TREC};

public:
	bool 					load;
	bool 					verbose;
	bool					debug;

	std::vector<int> 		skip;
	int 					skip_high;
	int 					skip_low;
	int 					to_skip; // for all skipping

	int 					max;

	stpl::Language 			lang;
	Format 					format;

	std::string 			wd;
	char  					sep[2];


	int 					mean;
	int 					mi;
	bool 					reward;

public:
	LIB_uniseg_settings();
	~LIB_uniseg_settings() {}
};
#endif /* __UNISEQ_SETTINGS_H__ */
