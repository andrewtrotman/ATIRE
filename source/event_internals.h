/*
	EVENT_INTERNALS.H
	-----------------
*/
#ifndef EVENT_INTERNALS_H_
#define EVENT_INTERNALS_H_


/*
	class ANT_EVENT_INTERNALS
	-------------------------
*/
class ANT_event_internals
{
public:
	#ifdef _MSC_VER
		HANDLE event;
	#else
		pthread_cond_t event;
		pthread_mutex_t mutex;
	#endif
	volatile long val;
} ;

#endif /* EVENT_INTERNALS_H_ */
