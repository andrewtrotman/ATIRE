/*
 * stop_word.c
 *
 *  Created on: Dec 22, 2009
 *      Author: monfee
 *      Using code from ANT stop_word.c by Andrew Trotman
 */

#include "stop_word.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

StopWord::StopWord() {
	// TODO Auto-generated constructor stub

}

StopWord::~StopWord() {
	// TODO Auto-generated destructor stub
}

int char_star_star_strcmp(const void *one, const void *two)
{
return strcmp(*(char **)one, *(char **)two);
}

bool StopWord::is_chinese_stop_word(const char *term)
{
//if (bsearch(&term, chinese_stop_word_list, chinese_stop_word_list_len, sizeof(*chinese_stop_word_list), char_star_star_strcmp) == NULL)
//	return false;
//
//return true;
	for (int i = 0; i < StopWord::chinese_stop_word_list_len; i++) {
		const char *stopword = StopWord::chinese_stop_word_list[i];
		if (strcmp(term, stopword) == 0)
			return true;
		//cout <<  stopword << endl;
	}
	return false;
}

char *StopWord::chinese_stop_word_list[] = {
		/*
	(char *)"的",
	(char *)"年",
	(char *)"一",
	(char *)"中",
	(char *)"是",
	(char *)"在",
	(char *)"大",
	(char *)"人",
	(char *)"国",
	(char *)"有",
	(char *)"日",
	(char *)"和",
	(char *)"月",
	(char *)"学",
	(char *)"一",
	(char *)"为",
	(char *)"了",
	(char *)"地",
	(char *)"成",
	(char *)"不",
	(char *)"國",
	(char *)"行",
	(char *)"上",
	(char *)"生",
	(char *)"斯",
	(char *)"主",
	(char *)"他",
	(char *)"為"，
	(char *)"之"
	*/
	(char *)"\347\232\204",
	(char *)"\345\271\264",
	(char *)"\344\270\200",
	(char *)"\344\270\255",
	(char *)"\346\230\257",
	(char *)"\345\234\250",
	(char *)"\345\244\247",
	(char *)"\344\272\272",
	(char *)"\345\233\275",
	(char *)"\346\234\211",
	(char *)"\346\227\245",
	(char *)"\345\222\214",
	(char *)"\346\234\210",
	(char *)"\345\255\246",
	(char *)"\344\270\200",
	(char *)"\344\270\272",
	(char *)"\344\272\206",
	(char *)"\345\234\260",
	(char *)"\346\210\220",
	(char *)"\344\270\215",
	(char *)"\345\234\213",
	(char *)"\350\241\214",
	(char *)"\344\270\212",
	(char *)"\347\224\237",
	(char *)"\346\226\257",
	(char *)"\344\270\273",
	(char *)"\344\273\226",
	(char *)"\347\202\272",
	(char *)"\344\271\213"
} ;

long StopWord::chinese_stop_word_list_len = sizeof(StopWord::chinese_stop_word_list) / sizeof(*StopWord::chinese_stop_word_list);

