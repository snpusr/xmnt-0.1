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

#ifndef NET_H
#define NET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct {
	int fd;
	int connected;
	int binded;
	struct sockaddr_in sock;
} net_t;

/* UDP */
extern int net_udp_server (net_t *n, unsigned short int port);
extern int net_udp_client (net_t *n, unsigned short int port, const char *addr);
//extern int net_udp_send   (net_t *n, const char *fmt, ...);
extern int net_udp_send_raw   (int fd, struct sockaddr_in *s, void *buffer, int size);
extern int net_udp_send   (int fd, struct sockaddr_in *s, const char *fmt, ...);
extern int net_udp_recv   (net_t *n, int timeout, struct sockaddr_in *from,
			   char *buffer, ssize_t lenght);
extern void net_udp_finish (net_t *n);

/* TCP */
extern int net_tcp_server (net_t *n, unsigned short int port);
extern int net_tcp_client (net_t *n, unsigned short int port, const char *addr);
extern int net_tcp_accept (net_t *n, net_t *c, int timeout);
extern int net_tcp_send   (net_t *c, const char *fmt, ...);
extern int net_tcp_write (net_t *c, void *, int);
extern int net_tcp_recv   (net_t *c, int timeout, char *buffer, ssize_t lenght);
extern int net_tcp_recv_line   (net_t *c, int timeout, char *buffer, ssize_t lenght);
extern void net_tcp_finish (net_t *n);
extern int net_tcp_bind_client(net_t *n, unsigned short src, unsigned short dst, const char *addr);

extern int net_nonblock(net_t *n, int opt);
extern int net_set_flags(net_t *n);

#endif /* net.h */
