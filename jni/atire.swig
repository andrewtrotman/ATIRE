/*
	ATIRE.SWIG
	---------------------
	This is the SWIG interface file for the all ATIRE APIs.
	see swig.org for information on SWIG
*/

%module atire_apis
%{
#include "../atire/atire_api_remote.h"
%}

%include "../atire/atire_api_remote.h"
int index(int argc, char *argv[]);


