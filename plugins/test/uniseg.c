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
	array_type	seged_words;

	if (argc > 1) {
		Seger	seger(argv[1]);
		seger.start();
		seger.add_to_list(seged_words);

		for (int i = 0; i < seged_words.size(); i++)
			cout << seged_words[i]->chars() << " ";
		cout << endl;
	}
	return 0;
}
