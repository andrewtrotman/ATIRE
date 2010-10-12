/*
 * application_out.cpp
 *
 *  Created on: Sep 16, 2010
 *      Author: monfee
 */

#include "application_out.h"
#include <iostream>

#include <string.h>

using namespace std;

namespace QLINK
{

application_out aout;

application_out::application_out()
{
	out_ = &cout;
}

application_out::~application_out()
{

}

//void application_out::operator << (const char *content)
//template< typename T >
//application_out &application_out::operator << (T const &content)
//{
//	T *t = 0;
//	buf_ << content;
//	return *this;
//}

void application_out::use_stdout(bool b)
{
	use_stdout_ = b;
	if (use_stdout_) {
		out_ = &cout;
	}
}

void application_out::reset()
{
	buf_.str("");
}

void application_out::printbuf(const char *content)
{
	buf_ << content;
}

void application_out::flush()
{
	*out_ << buf_.str();
}
}
