/*
 * cmdline_options.h
 *
 *  Created on: Aug 11, 2009
 *      Author: monfee
 */

#ifndef CMDLINE_OPTIONS_H_
#define CMDLINE_OPTIONS_H_

class cmdline_options
{
public:
	static int argc;
	static char **argv;

public:
	cmdline_options();
	virtual ~cmdline_options();
};

#endif /* CMDLINE_OPTIONS_H_ */
