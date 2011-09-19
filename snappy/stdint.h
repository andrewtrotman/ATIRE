/*
	STDINT.H
	--------
	Microsoft Visual Studio 2008 doesn't have stdint,h so this acts as a substitute
*/
#ifndef ANT_STDINT_H_
#define ANT_STDINT_H_

#if _MSC_VER <= 1500

	#include "../source/fundamental_types.h"

#else

	#include <stdint.h>

#endif

#endif /* ANT_STDINT_H_ */
