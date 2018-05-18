//
// Created by jslater on 17/04/18.
//

#ifndef SOCKETS_H
#define SOCKETS_H

#include "procinfo.h"

#ifdef __cplusplus
extern "C" {
#endif

extern
int
sck_set_nonblocking(fd_t sfd);

extern
int
sck_create_socketpair(fd_t *sfds);

extern
ssize_t
sck_ipc_send(fd_t sfd, struct procinfo_t *procinfo, size_t size);

extern
ssize_t
sck_ipc_recv(fd_t sfd, struct procinfo_t *procinfo, size_t size);

extern
int
sck_create_socket(int domain);

extern
int
sck_bind_socket(fd_t sfd, sa_family_t af, char const *host, uint16 port, uint16 *port_out);

extern
int
sck_listen(fd_t sfd);

#ifdef __cplusplus
}
#endif

#endif //SOCKETS_H
