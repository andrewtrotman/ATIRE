/*
	SOCKETS.C
	---------
*/
#include "str.h"
#include "maths.h"
#include <limits>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstddef>

#ifdef _MSC_VER
	#include <winsock2.h>
	#undef max
#else
	#include <errno.h>
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/time.h>
	#include <sys/ioctl.h>
	#include <fcntl.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <signal.h>

	#define INVALID_SOCKET (-1)
	#define SOCKET_ERROR (-1)

	typedef int SOCKET;
	typedef struct sockaddr_in SOCKADDR_IN;
	typedef struct sockaddr *PSOCKADDR;
	typedef fd_set FD_SET;

	#define closesocket ::close
	#define ioctlsocket ::ioctl
	#ifndef FIONREAD
		#define FIONREAD I_NREAD
	#endif
#endif

#ifdef __sun
     #include <stropts.h>
     #include <sys/conf.h>
#endif

#include "sockets.h"
#include "sockets_internals.h"

/*
	You get an warning from FD_SET() and so we disable it
*/
#include "pragma.h"
#pragma ANT_PRAGMA_CONST_CONDITIONAL

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

#ifdef _MSC_VER
	WSADATA *ANT_socket_internals::wsaData = NULL;
#else
	long ANT_socket_internals::signal_setup = 0;
#endif

long ANT_socket_internals::reference_count = 0;

/*
	ANT_SOCKET::ANT_SOCKET()
	------------------------
*/
ANT_socket::ANT_socket()
{
internals = new ANT_socket_internals;
poll_getsz_last_pos = buffer = NULL;
buffer_start = buffer_size = buffer_alloc_size = 0;
chunk_size = 2048;
internals->incoming_connection = internals->sock = INVALID_SOCKET;
gets_result = NULL;

#ifdef _WIN32
	if (internals->wsaData == NULL)
		{
		internals->wsaData = new WSADATA;

		if (WSAStartup(MAKEWORD(1, 1), internals->wsaData) == 0)
			if (LOBYTE(internals->wsaData->wVersion) != 1 || HIBYTE(internals->wsaData->wVersion) != 1)
				{
				delete internals->wsaData;
				internals->wsaData = NULL;
				WSACleanup();
				}
		}
#else
	if (!internals->signal_setup)
		{
		signal(SIGPIPE, SIG_IGN);
		internals->signal_setup = 1;
		}
#endif

internals->reference_count++;
}

/*
	ANT_SOCKET::~ANT_SOCKET()
	-------------------------
*/
ANT_socket::~ANT_socket()
{
close();

delete [] buffer;

internals->reference_count--;

#ifdef _MSC_VER
	if (internals->reference_count == 0)
		{
		delete internals->wsaData;
		internals->wsaData = NULL;
		WSACleanup();
		}
#endif

delete internals;
}

/*
	ANT_SOCKET::NON_BLOCKING()
	--------------------------
*/
void ANT_socket::non_blocking(void)
{
#ifdef _MSC_VER
	unsigned long mode = 1;
	ioctlsocket(internals->sock, FIONBIO, &mode);
#else
	fcntl(internals->sock, F_SETFL, fcntl(internals->sock, F_GETFL, 0) | O_NONBLOCK);
#endif
}

/*
	ANT_SOCKET::BLOCKING()
	----------------------
*/
void ANT_socket::blocking(void)
{
#ifdef _MSC_VER
	unsigned long mode = 0;
	ioctlsocket(internals->sock, FIONBIO, &mode);
#else
	fcntl(internals->sock, F_SETFL, fcntl(internals->sock, F_GETFL, 0) & ~O_NONBLOCK);
#endif
}

/*
	ANT_SOCKET::ERROR()
	-------------------
*/
int ANT_socket::error(void)
{
return internals->sock == INVALID_SOCKET;
}

/*
	ANT_SOCKET::OPEN()
	------------------
*/
int ANT_socket::open(const char *address, unsigned short port)
{
SOCKADDR_IN in;
struct hostent *host;
int connected = FALSE;

if ((internals->sock = socket(PF_INET, SOCK_STREAM, 0)) != INVALID_SOCKET)
	{
	in.sin_family = AF_INET;
	in.sin_port = htons(port);
	if ((host = gethostbyname(address)) != NULL)
		{
		memcpy(&(in.sin_addr), host->h_addr, host->h_length);
		if (connect(internals->sock, (PSOCKADDR)&in, sizeof(in)) != SOCKET_ERROR)
			connected = TRUE;
		}
	}

return connected;
}

/*
	ANT_SOCKET::LISTEN()
	--------------------
*/
int ANT_socket::listen(unsigned short port)
{
int connected = FALSE;
SOCKADDR_IN in;

if (internals->incoming_connection == INVALID_SOCKET)
	{
	/*
		First time around and so we play the socket(), bind(), listen() game
	*/
	internals->incoming_connection = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	in.sin_family = AF_INET;
	in.sin_port = htons(port);
	in.sin_addr.s_addr = INADDR_ANY;
	::bind(internals->incoming_connection, (PSOCKADDR)&in, sizeof(in));
	::listen(internals->incoming_connection, SOMAXCONN);
	}

if ((internals->sock = ::accept(internals->incoming_connection, NULL, NULL)) != INVALID_SOCKET)
	connected = TRUE;

return connected;
}

/*
	ANT_SOCKET::POLL_LISTEN()
	-------------------------
*/
ANT_socket *ANT_socket::poll_listen(unsigned short port)
{
SOCKADDR_IN in;
ANT_socket *clone;
SOCKET sock;

if (internals->sock == INVALID_SOCKET)
	if ((internals->sock = ::socket(AF_INET, SOCK_STREAM, 0)) != SOCKET_ERROR)
		{
		in.sin_family = AF_INET;
		in.sin_port = htons(port);
		in.sin_addr.s_addr = INADDR_ANY;
		if ((::bind(internals->sock, (PSOCKADDR)&in, sizeof(in)) == SOCKET_ERROR) || (::listen(internals->sock, SOMAXCONN) == SOCKET_ERROR))
			{
			close();
			return NULL;
			}
		}

#ifdef _MSC_VER
	struct timeval timeout = {0, 0};
	FD_SET fd_read;

	FD_ZERO(&fd_read);
	FD_SET(internals->sock, &fd_read);

	if (select(1, &fd_read, NULL, NULL, &timeout) != 0)
		{
		non_blocking();
		sock = ::accept(internals->sock, NULL, NULL);	// will block if socket has died between select and accept unless in non-blocking mode
		blocking();

		if (sock != INVALID_SOCKET)
			{
			clone = new ANT_socket();
			clone->internals->sock = sock;
			clone->blocking();
			return clone;
			}

		close();				// socket open failed for some reason!!!
		}
#else
	non_blocking();
	sock = accept(internals->sock, NULL, NULL);	// will block if socket has died between select and accept unless in non-blocking mode
	blocking();

	if (sock != INVALID_SOCKET)
		{
		clone = new ANT_socket();
		clone->internals->sock = sock;
		clone->blocking();
		return clone;
		}
	else
		if (errno != EWOULDBLOCK)
			close();
#endif

return NULL;
}

/*
	ANT_SOCKET::CLOSE()
	-------------------
*/
int ANT_socket::close(void)
{
linger die_now = {1, 0};

if (internals->sock != INVALID_SOCKET)
	{
	setsockopt(internals->sock, SOL_SOCKET, SO_LINGER, (char *)&die_now, sizeof(die_now));
	closesocket(internals->sock);
	internals->sock = INVALID_SOCKET;
	}
return 0;
}

/*
	ANT_SOCKET::TOP_UP_CACHE()
	--------------------------
*/
int ANT_socket::top_up_cache(int mode)
{
FD_SET which, err;
struct timeval timeout = {0, 0};
unsigned long stuff = 0;
int bytes_recvd = 0, read = 0, select_val = 0, ioctl_result = 0;
long long old_buffer_size;

if (internals->sock != INVALID_SOCKET)
	{
	FD_ZERO(&which);
	FD_SET(internals->sock, &which);
	FD_ZERO(&err);
	FD_SET(internals->sock, &err);

	select_val = ::select((int)internals->sock + 1, &which, NULL, &err, mode == BLOCK ? NULL : &timeout);

	/*
		OK what did we get from the select?  Presumably instruction to actually fill
		the cache as this is why we were called in the first place.  An error might
		still occur!!!
	*/
	if (select_val < 0)		// error
		close();
	else if (select_val > 0)
		{
		if (FD_ISSET(internals->sock, &which))
			{
			ioctl_result = ioctlsocket(internals->sock, FIONREAD, &stuff);
			if (ioctl_result != -1 && stuff != 0)
				{
				/*
					according to the book, "stuff" is the amount that can be read in one
					recv() not the amount ready to be read (which might be larger).  This
					however is incorrect so we have to read in a loop anyway!
				*/
				if (stuff > (unsigned long)(buffer_alloc_size - buffer_size))
					{
					old_buffer_size = buffer_alloc_size;
					buffer_alloc_size += chunk_size > (int)stuff ? chunk_size : (int)stuff;
					chunk_size += chunk_size;		// double the chunk_size each time.
					buffer = strrenew(buffer, sizeof(*buffer) * old_buffer_size, sizeof(*buffer) * buffer_alloc_size);
					}
				do
					{
					bytes_recvd = recv(internals->sock, buffer + buffer_size, (int)stuff, 0);

					if (bytes_recvd <= 0)
						{
						close();
						return -1;
						}
					else
						{
						read += bytes_recvd;
						stuff -= bytes_recvd;
						buffer_size += bytes_recvd;
						}
					}
				while (stuff > 0);

				return read;
				}
			else
				close();
			}
		else if (FD_ISSET(internals->sock, &err))
			close();
		}
	}
return -1;
}

/*
	ANT_SOCKET::REBASE_CACHE()
	--------------------------
*/
int ANT_socket::rebase_cache(void)
{
if (buffer_start == 0)
	return 0;

if (buffer_size - buffer_start != 0)
	memmove(buffer, buffer + buffer_start, (size_t)(buffer_size - buffer_start));
buffer_size -= buffer_start;
buffer_start = 0;
return 0;
}

/*
	ANT_SOCKET::GETSZ()
	-------------------
*/
char *ANT_socket::getsz(char termination)
{
char *from, *copy;
long long done;
int found = FALSE;

rebase_cache();
done = buffer_start;
do
	{
	from = buffer + done;
	while (from < buffer + buffer_size)
		if (*from == termination)
			{
			found = TRUE;
			break;
			}
		else
			from++;
	done = buffer_size;

	if (found)
		{
		copy = new char [from - buffer + 1];
		memcpy(copy, buffer, from - buffer + 1);
		copy[from - buffer] = '\0';
		buffer_start = from - buffer + 1;
		return copy;
		}

	if (top_up_cache() < 0)
		return NULL;
	}
while (1);

#ifndef __INTEL_COMPILER
	return NULL;
#endif
}

/*
	ANT_SOCKET::POLL_GETSZ()
	------------------------
*/
char *ANT_socket::poll_getsz(char termination)
{
char *from, *copy;
long long old_base, diff;

old_base = buffer_start;
rebase_cache();
if (top_up_cache(POLL) <= 0)
	return NULL;

diff = old_base - buffer_start;

if (poll_getsz_last_pos == NULL)
	poll_getsz_last_pos = buffer + buffer_start;
else
	poll_getsz_last_pos -= diff;

from = poll_getsz_last_pos;

while (from < buffer + buffer_size)
	if (*from == termination)
		{
		copy = new char [from - buffer + 1];
		memcpy(copy, buffer, from - buffer + 1);
		copy[from - buffer] = '\0';

		buffer_start = from - buffer + 1;
		poll_getsz_last_pos = NULL;
		return copy;
		}
	else
		from++;

poll_getsz_last_pos = from;

return NULL;
}

/*
	ANT_SOCKET::BLOCK_READ()
	------------------------
*/
char *ANT_socket::block_read(char *into, long long len)
{
rebase_cache();

while (buffer_size < len)
	if (top_up_cache() < 0)
		return 0;

if (len != 0)
	memcpy(into, buffer, (size_t)len);
buffer_start = len;

return into;
}

/*
	ANT_SOCKET::GETL()
	------------------
*/
long ANT_socket::getl(void)
{
int ans;

if ((block_read((char *)&ans, 4)) != NULL)
	return ntohl(ans);
return 0;
}

/*
	ANT_SOCKET::GETCH()
	-------------------
*/
char ANT_socket::getch(void)
{
char mem, *ans;

ans = block_read(&mem, 1);

return ans == NULL ? 0 : mem;
}

/*
	ANT_SOCKET::BLOCK_WRITE()
	-------------------------
*/
long long ANT_socket::block_write(const char *pbuffer, long long size)
{
const char *from;
unsigned long long len, sent;
int should_send;

len = size;
from = pbuffer;

while (len != 0)
	{
	should_send = (int)ANT_min(len, (unsigned long long)std::numeric_limits<int>::max());
	if ((sent = send(internals->sock, from, should_send, 0)) == SOCKET_ERROR)
		return -1;
	len -= sent;
	from += sent;
	}

return size;
}

/*
	ANT_SOCKET::PUTS()
	------------------
*/
long long ANT_socket::puts(char *pbuffer)
{
return block_write(pbuffer, strlen(pbuffer));
}

/*
	ANT_SOCKET::PUTCH()
	-------------------
*/
long long ANT_socket::putch(char ch)
{
return block_write(&ch, 1);
}

/*
	ANT_SOCKET::PUTSZ()
	-------------------
*/
long long ANT_socket::putsz(char *string)
{
return block_write(string, strlen(string) + 1);		// include the zero terminatin char.
}

/*
	ANT_SOCKET::PUTL()
	------------------
*/
long long ANT_socket::putl(long what)
{
unsigned int ans;

ans = htonl(what);

return block_write((char *)&ans, sizeof(ans));
}

/*
	ANT_SOCKET::WNTOHL()
	--------------------
*/
unsigned int ANT_socket::wntohl(unsigned int num)
{
return ntohl(num);
}

/*
	ANT_SOCKET::WHTONL()
	--------------------
*/
unsigned int ANT_socket::whtonl(unsigned int num)
{
return htonl(num);
}

