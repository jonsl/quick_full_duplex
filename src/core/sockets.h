//
// Created by jslater on 17/04/18.
//

#ifndef VIA_SOCKETS_H
#define VIA_SOCKETS_H

#include "config.h"

namespace via {
namespace sockets {

struct channel_t {
    uint32 command_;
    pid_t pid_;
    size_t slot_;
    fd_handle_t fd_;
};

extern
void
set_nonblocking(socket_t socket);

extern
void
ipc_prepare(socket_t *sfd);

extern
void
ipc_send(socket_t socket, channel_t *channel, size_t size); // send fd by socket

extern
void
ipc_recv(socket_t socket, channel_t *channel, size_t size); // receive fd from socket

extern
socket_t
prepare_listen_socket(uint16 port, int *reuse_addr);

}
}

#endif //VIA_SOCKETS_H
