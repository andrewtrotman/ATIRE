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
	unsigned char **output = NULL;
	unsigned char *word = NULL;
	int count = ANT_plugin_manager::instance().do_segmentation((unsigned char *)input.c_str(), output);
	int i = 0;
	while (i < count && (word = output[i]))
		{
		cout << word << endl;
		i++;
		}
	cout << endl;
	}

return 0;
}
