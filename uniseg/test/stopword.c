/*
 * stopword.cpp
 *
 *  Created on: Dec 22, 2009
 *      Author: monfee
 */

#include "../stop_word.h"

#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
	for (int i = 0; i < StopWord::chinese_stop_word_list_len; i++) {
		char *stopword = StopWord::chinese_stop_word_list[i];
		cout <<  stopword << endl;
	}
	return 0;
}
