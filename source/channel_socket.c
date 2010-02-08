/*
	CHANNEL_SOCKET.C
	----------------
*/
#include "str.h"
#include "channel_socket.h"
#include "sockets.h"

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_CHANNEL_SOCKET::ANT_CHANNEL_SOCKET()
	----------------------------------------
*/
ANT_channel_socket::ANT_channel_socket(unsigned short port, char *address) : ANT_channel()
{
connected = FALSE;
socket = new ANT_socket;
this->port = port;
this->address = address ==  NULL ? NULL : strnew(address);
}

/*
	ANT_CHANNEL_SOCKET::~ANT_CHANNEL_SOCKET()
	-----------------------------------------
*/
ANT_channel_socket::~ANT_channel_socket()
{
delete socket;
delete [] address;
}

/*
	ANT_CHANNEL_SOCKET::CONNECT()
	-----------------------------
*/
void ANT_channel_socket::connect(void)
{
if (!connected)
	{
	if (address == NULL)
		socket->listen(port);			// we're a server
	else
		socket->open(address, port);	// we're a client
	connected = TRUE;
	}
}

/*
	ANT_CHANNEL_SOCKET::BLOCK_WRITE()
	---------------------------------
	returns the number of bytes written or -ve on errror
*/
long ANT_channel_socket::block_write(char *data, long length)
{
connect();
return socket->block_write(data, length);
}

/*
	ANT_CHANNEL_SOCKET::BLOCK_READ()
	--------------------------------
*/
char *ANT_channel_socket::block_read(char *into, long length)
{
connect();
return socket->block_read(into, length);
}

/*
	ANT_CHANNEL_SOCKET::GETSZ()
	---------------------------
*/
char *ANT_channel_socket::getsz(char terminator)
{
connect();
return socket->getsz(terminator);
}
