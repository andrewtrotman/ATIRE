/*
	BARRIER.C
	---------
*/

#include "barrier.h"

/*
	ANT_BARRIER::ANT_BARRIER()
	--------------------------
*/
ANT_barrier::ANT_barrier(long count)
{
this->count = count;
event.clear();
}

/*
	ANT_BARRIER::SIGNAL()
	---------------------
*/
void ANT_barrier::signal(void)
{
critter.enter();
count--;
if (count <= 0)
	{
	count = 0;
	event.signal();
	}
critter.leave();
}

/*
	ANT_BARRIER::WAIT()
	-------------------
*/
void ANT_barrier::wait(void)
{
event.wait();
}

/*
	ANT_BARRIER::RESET()
	--------------------
*/
void ANT_barrier::reset(long count)
{
critter.enter();
this->count = count;
event.clear();
critter.leave();
}
