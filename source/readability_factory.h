/*
	READABILITY_FACTORY.H
	----------------------
*/

#ifndef __READABILITY_FACTORY_H__
#define __READABILITY_FACTORY_H__

/*
	class ANT_READABILITY_FACTORY
	-----------------------------
*/
class ANT_readability_factory
{
public:
	enum {
		NONE       = 1,
		DALE_CHALL = 2
	} ;
	
	ANT_readability_factory();
	ANT_readability_factory(unsigned long measure);
	virtual ~ANT_readability_factory();

private:
	unsigned long measures_to_use;

} ;

#endif __READABILITY_FACTORY_H__
