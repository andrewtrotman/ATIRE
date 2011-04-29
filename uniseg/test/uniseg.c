/*
 * uniseg.c
 *
 *  Created on: Jul 9, 2009
 *      Author: monfee
 */

#include "seg.h"
#include <iostream>

using namespace std;

void usage(char *program)
{
	fprintf(stderr, "Usage: %s chinese_text\n", program);
	exit(-1);
}

int main(int argc, char **argv)
{
	if (argc > 1) {
		Seger	seger(argv[1]);
		seger.start();

		const unsigned char* output = seger.output();
		unsigned char *next = (unsigned char *)output;
		while (*next != '\0')
			{
			string word;
			while (*next != ' ')
				{
				word.push_back(*next);
				next++;
				}
			next++;
			cout << word << endl;
			}
	}
	else
		usage(argv[0]);
	return 0;
}
