/*
	SEMAPHORE.H
	-----------
*/
#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

class ANT_semaphore_internals;

/*
	class ANT_SEMAPHORE
	-------------------
*/
class ANT_semaphore
{
private:
	ANT_semaphore_internals *internals;

public:
	ANT_semaphore(long initial, long maximum);
	virtual ~ANT_semaphore();

	void enter(void);
	void leave(void);
	long poll(void);
} ;


#endif /* SEMAPHORE_H_ */



