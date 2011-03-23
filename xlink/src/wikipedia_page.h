/*
 * wikipedia_page.h
 *
 *  Created on: Mar 22, 2011
 *      Author: monfee
 */

#ifndef WIKIPEDIA_PAGE_H_
#define WIKIPEDIA_PAGE_H_

namespace QLINK
{

class wikipedia_page
{
private:
	static char *century_list[];
	static long century_list_len;

public:
	wikipedia_page();
	virtual ~wikipedia_page();

	bool is_century(const char *term);
	bool is_year(const char *term);
	bool is_number(const char *term);
};

}

#endif /* WIKIPEDIA_PAGE_H_ */
