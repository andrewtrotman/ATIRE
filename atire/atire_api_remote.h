/*
	ATIRE_API_REMOTE.H
	------------------
*/
#ifndef ATIRE_API_REMOTE_H_
#define ATIRE_API_REMOTE_H_

class ANT_socket;

#ifndef NULL
	#define NULL 0
#endif


/*
	class ATIRE_API_REMOTE
	----------------------
*/
class ATIRE_API_remote
{
private:
	char *connect_string;
	ANT_socket *socket;

public:
	/*
		ATIRE_API_remote
	*/
	ATIRE_API_remote(void);
	virtual ~ATIRE_API_remote();

	/*
		Open a connetion to a remote server.
		The connect_string is in the format blarg.com:port where the default port number is 8088
	*/
	long open(char *connect_string);

	/*
		Close the connection
	*/
	long close(void);

	/*
		Testing method, return the connect_string.
	*/
	char *get_connect_string(void);

	/*
		Load a new doclist and index by filename.
	*/
	virtual long load_index(char *doclist_filename, char *index_filename);

	/*
		Return an XML document describing the index that is currently loaded (incl filenames).
	*/
	virtual char *describe_index();

	/*
		Search and return page_length results starting from top_of_page in the results list
		normally this would be 10 results from position 1
	*/
	virtual char *search(char *query, long long top_of_page, long long page_length, char *ranker=NULL);

	/*
		Given the ATIRE internal id of a document, retrieve that document (if it is in the repository)
	*/
	virtual char *get_document(long long docid, long long *length);
} ;

#endif /* ATIRE_API_REMOTE_H_ */
