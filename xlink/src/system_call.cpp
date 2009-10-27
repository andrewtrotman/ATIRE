/*
 * system_call.cpp
 *
 *  Created on: Sep 22, 2009
 *      Author: monfee
 */

#include "system_call.h"

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

using namespace QLINK;

system_call::system_call()
{

}

system_call::~system_call()
{

}

void system_call::cmd(char *cmd_str)
{
	strcpy(cmd_buf_, cmd_str);
}

char *system_call::execute(const char *cmd)
{
	reset();
	dl_ = popen(cmd, "r");
	if(!dl_){
	  perror("popen");
	  exit(-1);
	}
	else {
		fgets(result_buf_, BUF_SIZE, dl_);
	}
	pclose(dl_);

	int idx = strlen(result_buf_) - 1;
	while (idx > 0) {
		if (isspace(result_buf_[idx]))
			result_buf_[idx] = '\0';
		else
			break;
		--idx;
	}
	return result_buf_;
}
