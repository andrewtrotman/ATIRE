/*
	CRITICAL_SECTION_INTERNALS.H
	----------------------------
*/
#ifndef CRITICAL_SECTION_INTERNALS_H_
#define CRITICAL_SECTION_INTERNALS_H_

/*
	class ANT_CRITICAL_SECTION_INTERNALS
	------------------------------------
*/
class ANT_critical_section_internals
{
public:
	#ifdef NT_CRITICAL_SECTIONS
		CRITICAL_SECTION mutex;
	#else
		pthread_mutex_t mutex;
	#endif
	volatile long level;
} ;


#endif /* CRITICAL_SECTION_INTERNALS_H_ */
