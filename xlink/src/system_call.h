/*
 * system_call.h
 *
 *  Created on: Sep 22, 2009
 *      Author: monfee
 */

#ifndef SYSTEM_CALL_H_
#define SYSTEM_CALL_H_

#include <stdio.h>

namespace QLINK {
	class system_call
	{
	public:
		static unsigned const int BUF_SIZE = 1024;

	private:
		FILE *dl_;
		char result_buf_[BUF_SIZE];
		char cmd_buf_[BUF_SIZE];

	public:
		system_call();
		virtual ~system_call();

		void cmd(char *cmd_str);
		char *result() { return result_buf_; }
		char *execute(const char *cmd);
		char *execute() { return execute(cmd_buf_); }
		static system_call& instance();

	private:
		void reset();
	};

	inline system_call& system_call::instance() {
		static system_call inst;
		return inst;
	}

	inline void system_call::reset() {
		*result_buf_ = '\0';
	}
}

#endif /* SYSTEM_CALL_H_ */
