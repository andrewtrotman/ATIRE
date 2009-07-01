/*
	READABILITY_NONE.H
	------------------
*/
#ifndef __READABILITY_NONE_H__
#define __READABILITY_NONE_H__

#include "readability.h"

/*
	class ANT_READABILITY_NONE
	--------------------------
*/
class ANT_readability_none : public ANT_readability
{
public:
	ANT_readability_none() {};
	virtual ~ANT_readability_none() {};

	virtual long score();
} ;

#endif __READABILITY_NONE_H__
