/*
	SOCKETS.H
	---------
*/
#ifndef __SOCKETS_H__
#define __SOCKETS_H__

class ANT_socket_internals;

/*
	class ANT_SOCKET
	----------------
*/
class ANT_socket
{
protected:
	enum { BLOCK, POLL } ;

protected:
	ANT_socket_internals *internals;
	char *buffer;
	long long buffer_start, buffer_size, buffer_alloc_size, chunk_size;
	char *poll_getsz_last_pos;
	unsigned short port;
	char *gets_result;

protected:
	int top_up_cache(int mode = BLOCK);
	int rebase_cache(void);
	void blocking(void);
	void non_blocking(void);

public:
	ANT_socket();
	virtual ~ANT_socket();

	int open(const char *address, unsigned short port);
	int listen(unsigned short port);
	ANT_socket *poll_listen(unsigned short port);
	int close(void);
	int error(void);

	char *block_read(char *into, long long len);
	char *getsz(char termination = '\0');
	char *poll_getsz(char termination = '\0');
	char *gets(void) { return getsz('\n'); }
	char *poll_gets(void) { return poll_getsz('\n'); }
	long getl(void);
	char getch(void);

	long long block_write(const char *buffer, long long size);
	long long puts(char *buffer);
	long long putch(char ch);
	long long putsz(char *string);
	long long putl(long what);

	unsigned int wntohl(unsigned int num);
	unsigned int whtonl(unsigned int num);
} ;

#endif /* __SOCKETS_H__ */
