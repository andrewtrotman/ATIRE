/*
	SEMAPHORES.H
	------------
*/
#ifndef SEMAPHORES_H_
#define SEMAPHORES_H_

class ANT_semaphores_internals;

/*
	class ANT_SEMAPHORES
	--------------------
*/
class ANT_semaphores
{
private:
	ANT_semaphores_internals *internals;

public:
	ANT_semaphores(long initial, long maximum);
	virtual ~ANT_semaphores();

	void enter(void);
	void leave(void);
	long poll(void);
} ;

#endif /* SEMAPHORES_H_ */
