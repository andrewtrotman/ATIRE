/*
 * application_out.h
 *
 *  Created on: Sep 16, 2010
 *      Author: monfee
 */

#ifndef APPLICATION_OUT_H_
#define APPLICATION_OUT_H_

#include <sstream>
#include <ostream>
#include "pattern_singleton.h"

namespace QLINK
{

	class application_out: public pattern_singleton<application_out>
	{
	public:

	private:
		std::stringstream	 	buf_;
		std::ostream			*out_;
		bool					use_stdout_;

	public:
		application_out();
		virtual ~application_out();

		void use_stdout(bool b);
		void printbuf(const char *content);
		void reset();
		void flush();

		//void operator << (const char *content);
		template <typename T>
		void operator << (T const &content) {
			buf_ << content;
		}
	};

	extern application_out	aout;
}

#endif /* APPLICATION_OUT_H_ */
