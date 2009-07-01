/*
	READABILITY.H
	-------------
*/

#ifndef __READABILITY_H__
#define __READABILITY_H__

/* 
	class ANT_READABILITY
	---------------------
*/
class ANT_readability
{
public:
	ANT_readability() {};
	virtual ~ANT_readability() {};

	virtual long score() = 0;
} ;

#endif __READABILITY_H__
