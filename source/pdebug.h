#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdlib.h>
#include <stdio.h>

#include <typeinfo>

#ifdef PDEBUG
	#define dbg_printf(...) \
		{	fprintf(stderr, "%s:%s(%d): ", __FILE__, __func__, __LINE__); \
			fprintf(stderr, __VA_ARGS__); \
			fflush(stderr); \
		}
	#define dbg_type_name(var) dbg_printf(#var "(type_name): %s\n", typeid(var).name())
	#define dbg_real_type_name(var) dbg_printf(#var "(real_type_name): %s\n", typeid(*var).name())
#else
	#define dbg_printf(...)
	#define dbg_type_name(var)
	#define dbg_real_type_name(var)
#endif


#endif /* DEBUG_H_ */
