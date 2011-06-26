/*
	ATIRE_BROKE_ENGINE.H
	--------------------
*/
#ifndef ATIRE_BROKE_ENGINE_H_
#define ATIRE_BROKE_ENGINE_H_

class ATIRE_API_remote;

/*
	class ATIRE_BROKE_ENGINE
	------------------------
*/
class ATIRE_broke_engine
{
protected:
	static const long MAX_RETRIES = 2;

protected:
	enum {QUIET, NOISEY};

private:
	char *connect_string;
	ATIRE_API_remote *server;

protected:
	long open_connection_to_server(long voice = NOISEY);

public:
	ATIRE_broke_engine(char *connect_string);
	virtual ~ATIRE_broke_engine();

	virtual char *search(char *query, long long top_of_page, long long length);
} ;


#endif /* ATIRE_BROKE_ENGINE_H_ */
