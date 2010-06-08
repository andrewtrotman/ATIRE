/*
	BARRIER.H
	---------
*/
#ifndef BARRIER_H_
#define BARRIER_H_

#include "critical_section.h"
#include "event.h"

/*
	class ANT_BARRIER
	-----------------
*/
class ANT_barrier
{
private:
	ANT_critical_section critter;
	ANT_event event;
	long count;
	
public:
	ANT_barrier(long count = 0);
	virtual ~ANT_barrier() {}

	void signal(void);
	void wait(void);
	void reset(long count);
} ;

#endif /* BARRIER_H_ */
