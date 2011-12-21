/*
	ATIRE_ENGINE_RESULT_SET.H
	-------------------------
*/
#ifndef ATIRE_ENGINE_RESULT_SET_H_
#define ATIRE_ENGINE_RESULT_SET_H_

class ATIRE_engine_result;

/*
	class ATIRE_ENGINE_RESULT_SET
	-----------------------------
*/
class ATIRE_engine_result_set
{
public:
	static const long long expansion_factor = 2;		// double the current size of the list on overflow
	ATIRE_engine_result *results;
	long long hits;
	long long length_of_results;

protected:
	void extend(long long new_size);
	static int cmp(const void *a, const void *b);

public:
	ATIRE_engine_result_set();
	virtual ~ATIRE_engine_result_set();

	virtual void rewind(void);
	virtual long long add(char *serialised_version, long long docid_base = 0);
	virtual void sort(void);
	virtual char *serialise(char *query, long long hits, long long time_taken, long long first, long long page_length);
} ;


#endif /* ATIRE_ENGINE_RESULT_SET_H_ */
