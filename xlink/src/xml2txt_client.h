/*
 * xml2txt_client.h
 *
 *  Created on: Sep 14, 2009
 *      Author: monfee
 */

#ifndef XML2TXT_CLIENT_H_
#define XML2TXT_CLIENT_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

class xml2txt_client
{
public:
	const static int PORT = 20099;
	const static int BUFFER_SIZE = 1024 * 1024;

	const static char *DEFAULT_HOST;

	const static char *XML2TXT_REQUEST_START;
	const static char *XML2TXT_REQUEST_REQUEST_START;
	const static char *XML2TXT_REQUEST_REQUEST_END;
	const static char *XML2TXT_REQUEST_CONTENT_SIZE_START;
	const static char *XML2TXT_REQUEST_CONTENT_SIZE_END;
	const static char *XML2TXT_REQUEST_END;

	const static char *XML2TXT_RETURN_START;
	const static char *XML2TXT_RETURN_CONTENT_SIZE_START;
	const static char *XML2TXT_RETURN_CONTENT_SIZE_END;
	const static char *XML2TXT_RETURN_END;

private:
    char hostname_[100];
//	char    dir[DIRSIZE];
	int	sd_;
	struct sockaddr_in serv_addr_;
	struct hostent *server_;

	char buffer_[BUFFER_SIZE];
	char *text_;
	long length_;

public:
	xml2txt_client();
	xml2txt_client(char *server_name);
	virtual ~xml2txt_client();

	bool connect_server();
	void start_request();
	void end_request();
	void send_request(char *xml);
	void recv_text();
	void close_connect() { close_socket(); }

	long length() { return length_; }
	char *buffer() { return buffer_; }
	char *text() { return text_; }
	void reset_buffer() { length_ = 1; buffer_[0] = '\0'; text_ = buffer_;}
	void debug_send(int sent);

	bool more() { return length_ == BUFFER_SIZE; }

private:
	void init();
	void create_socket();
	void close_socket();
	int send_buffer(const char *source);
	int recv_buffer(char *to, int length, int flag = 0, bool all = true);
};

#endif /* XML2TXT_CLIENT_H_ */
