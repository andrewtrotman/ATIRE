/*
	READABILITY_NONE.H
	------------------
*/

#ifndef __READABILITY_NONE_H__
#define __READABILITY_NONE_H__

/*
	class ANT_READABILITY_NONE
	--------------------------
*/
class ANT_readability_none : public ANT_readability
{
public:
	ANT_readability_none() { measure_name = new ANT_string_pair("~none"); }
	virtual ~ANT_readability_none() {}
	
	void index(ANT_memory_index *index) {}
} ;

#endif __READABILITY_NONE_H__
