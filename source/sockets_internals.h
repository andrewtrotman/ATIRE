/*
	SOCKETS_INTERNALS.H
	-------------------
*/
#ifndef __SOCKETS_INTERNALS_H__
#define __SOCKETS_INTERNALS_H__

/*
	class ANT_SOCKET_INTERNALS
	---------------------------
*/
class ANT_socket_internals
{
public:
#ifdef _WIN32
	static WSADATA *wsaData;
#else
	static long signal_setup;
#endif
	static long reference_count;
	SOCKET sock, incoming_connection;
} ;

#endif /*__SOCKETS_INTERNALS_H__*/
