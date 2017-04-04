/*
 * The MIT License
 * Copyright (c) 2002 Void Technologies
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#define __USE_GNU
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>

#include "net.h"

int net_tcp_client (net_t *n, unsigned short int port, const char *addr)
{
    fd_set         rfd,wfd;
    struct timeval tv = { 0, 0 };
	struct hostent *h;
	
	if (!(h = gethostbyname(addr)))
		return 0;

	n->connected = 0;
	n->sock.sin_family      = AF_INET;
	n->sock.sin_port        = htons (port);
	memcpy(&n->sock.sin_addr, h->h_addr, h->h_length);

	n->fd = socket (AF_INET, SOCK_STREAM, 0);
	if (n->fd < 0) return 0;

	if (connect (n->fd, (struct sockaddr *) &n->sock, sizeof (n->sock)) < 0) {
		return 0;
    }
	else {
		n->connected = 1;
    }
    // bug ?
	//if(n->fd == 0)
	//	n->fd = 3;
	return 1;
}

int net_tcp_write(net_t *c, void *data, int dataLen)
{
	int r;
	if (!c->connected) return 0;
    if(write(c->fd, data, dataLen) != dataLen)
        return -1;
    return dataLen;
}

int net_tcp_recv(net_t *c, int timeout, char *buffer, ssize_t lenght)
{
	int r, s;
	fd_set fds;
	struct timeval tv;

	if (!c->connected) return 0;

	FD_ZERO (&fds);
	FD_SET (c->fd, &fds);

	tv.tv_sec = 0;
	tv.tv_usec = timeout * 10000;
	if ((s=select (c->fd + 1, &fds, NULL, NULL, !timeout ? NULL : &tv)) <= 0) return s;
	r = recv (c->fd, buffer, lenght, 0);
	buffer[r] = '\0';
	return r;
}

int net_tcp_recv_line(net_t *c, int timeout, char *buffer, ssize_t lenght)
{
	int r, s, offset;
	fd_set fds;
	struct timeval tv;

	if (!c->connected) return 0;

	FD_ZERO (&fds);
	FD_SET (c->fd, &fds);

	offset = tv.tv_sec = 0;
	tv.tv_usec = timeout * 1000;

	if ((s=select (c->fd + 1, &fds, NULL, NULL,
		!timeout ? NULL : &tv)) <= 0) { return s; }

    offset = 0;
    while((r = recv(c->fd, buffer + offset, 1, 0)) == 1) {
        if(offset < lenght) {
            if(buffer[offset] == '\n') {
                buffer[offset+1]=0;
                return ++offset;
            } else {
                offset += 1;
            }
        } else break;
    }
	return offset;
}

void net_tcp_finish (net_t *n)
{
	n->connected = 0;
	shutdown (n->fd, 2);
	close (n->fd);
}
