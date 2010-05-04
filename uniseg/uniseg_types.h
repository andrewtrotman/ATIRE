#ifndef __QTYPE_H__
#define __QTYPE_H__

#include "uniseg_string.h"

#ifdef WITH_ANT_PLUGIN
    #include "../source/plugin.h"
    typedef ANT_plugin								uniseg_plugin_interface;
#else
    class uniseg_plugin_interface {};
#endif
typedef uniseg_plugin_interface							*maker_t();

#endif /* __QTYPE_H_ */
