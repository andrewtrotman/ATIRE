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

		unsigned char** result = seger.output();
		int i = 0;
		while (result[i]) {
			cout << result[i] << " ";
			i++;
		}
		cout << endl;
	}
	return 0;
}
