/*
 * xml2txt_client.cpp
 *
 *  Created on: Sep 14, 2009
 *      Author: monfee
 */

#include "xml2txt_client.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

const char *xml2txt_client::DEFAULT_HOST = "localhost";

const char *xml2txt_client::XML2TXT_REQUEST_START = "<RST>";
const char *xml2txt_client::XML2TXT_REQUEST_REQUEST_START = "<REQUEST>";
const char *xml2txt_client::XML2TXT_REQUEST_REQUEST_END = "</REQUEST>";
const char *xml2txt_client::XML2TXT_REQUEST_CONTENT_SIZE_START = "<SIZE>";
const char *xml2txt_client::XML2TXT_REQUEST_CONTENT_SIZE_END = "</SIZE>";
const char *xml2txt_client::XML2TXT_REQUEST_END = "</RST>";

const char *xml2txt_client::XML2TXT_RETURN_START = "<RETURN><RETURN_START>##$$XML2TXT$$##</RETURN_START>";
const char *xml2txt_client::XML2TXT_RETURN_CONTENT_SIZE_START = "<SIZE>";
const char *xml2txt_client::XML2TXT_RETURN_CONTENT_SIZE_END = "</SIZE>";
const char *xml2txt_client::XML2TXT_RETURN_END = "<RETURN_END>##$$XML2TXT$$##</RETURN_END></RETURN>";

xml2txt_client::xml2txt_client()
{
    strcpy(hostname_, DEFAULT_HOST);
    init();
}

xml2txt_client::xml2txt_client(char *server_name)
{
    strcpy(hostname_, server_name);
    init();
}

xml2txt_client::~xml2txt_client()
{
	close_socket();
}

void xml2txt_client::init()
{
	length_ = 0;
	text_ = buffer_;

	/* go find out about the desired host machine */
	if ((server_ = gethostbyname(hostname_)) == 0) {
		perror("gethostbyname");
		exit(1);
	}

	/* fill in the socket structure with host information */
	memset(&serv_addr_, 0, sizeof(serv_addr_));
	serv_addr_.sin_family = AF_INET;
	serv_addr_.sin_addr.s_addr = ((struct in_addr *)(server_->h_addr))->s_addr;
	serv_addr_.sin_port = htons(PORT);

}

void xml2txt_client::create_socket()
{
	/* grab an Internet domain socket */
	if ((sd_ = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
}

void xml2txt_client::close_socket()
{
	close(sd_);
}

bool xml2txt_client::connect_server()
{
	create_socket();

	/* connect to PORT on HOST */
	if (connect(sd_, (struct sockaddr *) &serv_addr_, sizeof(serv_addr_)) == -1) {
		perror("connect");
		return false;
	}
	return true;
}

void xml2txt_client::start_request()
{
	send_buffer(XML2TXT_REQUEST_START);
}

void xml2txt_client::end_request()
{
	send_buffer(XML2TXT_REQUEST_END);
}

void xml2txt_client::send_request(char *xml)
{
	char size_buf[100];
	int size = strlen(xml);
	sprintf(size_buf, "%d", size);

//	send(sd_, XML2TXT_REQUEST_START, strlen(XML2TXT_REQUEST_START), 0);
//	send(sd_, XML2TXT_REQUEST_CONTENT_SIZE_START, strlen(XML2TXT_REQUEST_CONTENT_SIZE_START), 0);
//	send(sd_, size_buf, strlen(size_buf), 0);
//	send(sd_, XML2TXT_REQUEST_CONTENT_SIZE_END, strlen(XML2TXT_REQUEST_CONTENT_SIZE_END), 0);
//	send(sd_, xml, strlen(xml), 0);
//	send(sd_, XML2TXT_REQUEST_END, strlen(XML2TXT_REQUEST_END), 0);

	send_buffer(XML2TXT_REQUEST_REQUEST_START);
	send_buffer(XML2TXT_REQUEST_CONTENT_SIZE_START);
	send_buffer(size_buf);
	send_buffer(XML2TXT_REQUEST_CONTENT_SIZE_END);
	send_buffer(xml);
	send_buffer(XML2TXT_REQUEST_REQUEST_END);
}

void xml2txt_client::recv_text()
{
	//length_ = recv(sd_, buffer_, BUFFER_SIZE, 0);
	recv_buffer(buffer_, strlen(XML2TXT_RETURN_START));
	recv_buffer(buffer_, strlen(XML2TXT_RETURN_CONTENT_SIZE_START));

	char one_byte[2] = {'\0', '\0'};
	char size_buf[100];
	char *where = size_buf;
	do {
		recv_buffer(one_byte, 1);
		*where++ = *one_byte;
	} while (isdigit(*one_byte));

	length_ = atoi(size_buf);

	buffer_[0] = *one_byte;
	recv_buffer(buffer_ + 1, strlen(XML2TXT_RETURN_CONTENT_SIZE_END) - 1);
	recv_buffer(buffer_, length_);

	char *end = new char[strlen(XML2TXT_RETURN_END)];
	//recv(sd_, buffer_, BUFFER_SIZE, 0);
	recv_buffer(end, strlen(XML2TXT_RETURN_END));

	if (strncmp(end, XML2TXT_RETURN_END, strlen(XML2TXT_RETURN_END)) != 0)
		while (true) {
			if (recv_buffer(end, 1, MSG_DONTWAIT) <= 0)
				break;
		}
	delete [] end;
}

void xml2txt_client::debug_send(int sent)
{

}

int xml2txt_client::send_buffer(const char *source)
{
	int length = strlen(source);
	int sent = 0;
	char *from = (char *)source;

	while (sent < length) {
		from += sent;
		sent += send(sd_, from, length - sent, 0);
	}
	return sent;
}

int xml2txt_client::recv_buffer(char *to, int length, int flag, bool all)
{
	int recved = 0;
	int recved_onece = 0;
	char *from = (char *)to;

	while (recved < length) {
		from += recved;

		recved_onece = recv(sd_, from, length - recved, flag);
		if (recved_onece <= 0)
			return recved_onece;

		recved += recved_onece;
		if (!all)
			break;
	}
	to[recved] = '\0';
	return recved;
}

