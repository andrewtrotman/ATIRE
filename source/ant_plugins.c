/*
 * ANT_PLUGINS.C
 * ------------
 *
 *  Created on: Jul 13, 2009
 *      Author: monfee
 *
 *  test the plugins of ant, all the plugins is located in the "plugins" directory which should be as the same level of ant execution file
 */

#include <iostream>
#include <string>
#include "plugin_manager.h"

using namespace std;

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_plugin_manager::instance().load();

while(1)
	{
	string input;
	cout << "> ";
	cin >> input;
	const char *output = NULL;
	output = ANT_plugin_manager::instance().do_segmentation((unsigned char *)input.c_str());
	char *next = (char *)output;
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
