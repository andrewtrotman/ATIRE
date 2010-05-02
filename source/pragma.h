/*
	PRAGMA.H
	---------
	This is the neasty include file that is necessary in order to have
	consistent pragma handling throughout the ANT source code.  Nothing
	should be in this file except for #pragma directives used to hide
	compiler warnings.
*/
#ifndef PRAGMA_H_
#define PRAGMA_H_

#ifdef _MSC_VER
	#define ANT_PRAGMA_UNUSED_PARAMETER  warning(suppress:4100)		// unreferenced formal parameter
	#define ANT_PRAGMA_UNUSED_PARAMETER_DISABLE  warning(disable:4100)		// unreferenced formal parameter

	#define ANT_PRAGMA_CONST_CONDITIONAL warning(disable:4127)		// conditional expression is constant
	#define ANT_PRAGMA_UNREACHABLE_CODE  warning(disable:4702)		// unreachable code
#else
	#define ANT_PRAGMA_UNUSED_PARAMETER
	#define ANT_PRAGMA_UNUSED_PARAMETER_DISABLE
	#define ANT_PRAGMA_CONST_CONDITIONAL
	#define ANT_PRAGMA_UNREACHABLE_CODE
#endif


#endif /* PRAGMA_H_ */
