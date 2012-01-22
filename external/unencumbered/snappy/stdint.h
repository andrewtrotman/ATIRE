/*
	STDINT.H
	--------
	Microsoft Visual Studio 2008 doesn't have stdint,h so this acts as a stub
*/
#ifndef ANT_STDINT_H_
#define ANT_STDINT_H_

#ifdef _MSC_VER

	#include "../../../source/fundamental_types.h"

#else

	#include <stdint.h>

#endif

#endif /* ANT_STDINT_H_ */
