/*
	EVENT.H
	-------
*/
#ifndef EVENT_H_
#define EVENT_H_

class ANT_event_internals;

/*
	class ANT_EVENT
	---------------
*/
class ANT_event
{
protected:
	ANT_event_internals *internals;

public:
	ANT_event();
	virtual ~ANT_event();

	virtual void wait(void);
	virtual void signal(void);
	virtual void clear(void);
	virtual long poll(void);		// CLEARed=0   SIGNALed=1
	virtual void pulse(void);
} ;

#endif /* EVENT_H_ */
