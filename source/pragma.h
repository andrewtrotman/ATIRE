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
	#ifdef __INTEL_COMPILER
		#define ANT_PRAGMA_UNUSED_PARAMETER warning disable 869
		#define ANT_PRAGMA_UNUSED_PARAMETER_DISABLE warning disable 869

		#define ANT_PRAGMA_CONST_CONDITIONAL warning disable 4127
		#define ANT_PRAGMA_UNREACHABLE_CODE warning disable 4702

		#define ANT_PRAGMA_NO_DELETE warning disable 442
	#else
		#define ANT_PRAGMA_UNUSED_PARAMETER warning(suppress:4100)			// unreferenced formal parameter
		#define ANT_PRAGMA_UNUSED_PARAMETER_DISABLE  warning(disable:4100)	// unreferenced formal parameter

		#define ANT_PRAGMA_CONST_CONDITIONAL warning(disable:4127)			// conditional expression is constant
		#define ANT_PRAGMA_UNREACHABLE_CODE warning(disable:4702)			// unreachable code

		#define ANT_PRAGMA_NO_DELETE warning(disable:4291)					// the class has an operator new but not an operator delete
	#endif
#else
	#define ANT_PRAGMA_UNUSED_PARAMETER
	#define ANT_PRAGMA_UNUSED_PARAMETER_DISABLE
	#define ANT_PRAGMA_CONST_CONDITIONAL
	#define ANT_PRAGMA_UNREACHABLE_CODE
	#define ANT_PRAGMA_NO_DELETE
#endif

#endif /* PRAGMA_H_ */
