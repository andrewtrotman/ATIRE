/*
 * uniseg.c
 *
 *  Created on: Jul 9, 2009
 *      Author: monfee
 */

#include "seg.h"
#include <iostream>

using namespace std;

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
	return 0;
}
