/*
 * PLUGIN.H
 * --------
 *
 *  Created on: Jul 2, 2009
 *      Author: monfee
 */

#ifndef __PLUGIN_H__
#define __PLUGIN_H__

/*
	class ANT_PLUGIN
	----------------
*/
class ANT_plugin
{
public:
	enum plugin_type { SEGMENTATION  = 0 };

protected:
	char	*name;	// plugin library name, e.g segmentation module on linux named libuniseg.so, on Windows named uniseg.dll

public:
   virtual void load();
};

#endif __PLUGIN_H__
