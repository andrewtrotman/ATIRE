/*
	CRITICAL_SECTION.H
	------------------
*/
#ifndef CRITICAL_SECTION_H_
#define CRITICAL_SECTION_H_

class ANT_critical_section_internals;

/*
	class ANT_CRITICAL_SECTION
	--------------------------
*/
class ANT_critical_section
{
protected:
	ANT_critical_section_internals *internals;

public:
	ANT_critical_section();
	virtual ~ANT_critical_section();

	virtual void enter(void);
	virtual void leave(void);
} ;

#endif /* CRITICAL_SECTION_H_ */
