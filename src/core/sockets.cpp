//
// Created by jslater on 17/04/18.
//

#include "sockets.h"
#include "error.h"

namespace via {
namespace sockets {

void set_nonblocking(socket_t socket) {

    int r;

    r = fcntl(socket, F_GETFL);

    if (r < 0) {
        throw socket_set_nonblocking_failed_error();
    }

    r = fcntl(socket, F_SETFL, r | O_NONBLOCK);

    if (r < 0) {
        throw socket_set_nonblocking_failed_error();
    }
}

void
ipc_prepare(socket_t *sfd) {

    int r;

    r = ::socketpair(AF_UNIX, SOCK_STREAM, 0, sfd);

    if (r < 0) {
        throw ipc_socketpair_creation_failed_error();
    }

    set_nonblocking(sfd[0]);
}

/// send a channel list to another process
/// \param socket
/// \param channel
/// \param size
/// \return result of sendmsg()
void
ipc_send(socket_t socket, channel_t *channel, size_t size) {

    static constexpr int NUM_FD = 1;

    int fds[NUM_FD];  /* Contains the file descriptors to pass */

    struct ::iovec iov = {
            .iov_base = channel,
            .iov_len = size
    };
    union {         /* Ancillary data buffer, wrapped in a union
                              in order to ensure it is suitably aligned */
        struct ::cmsghdr cmsghdr_;
        char buf_[CMSG_SPACE(sizeof(fds))];

    } cmsg{};

    struct ::msghdr msg{};
    bzero(&msg, sizeof(msg));

    cmsg.cmsghdr_.cmsg_level = SOL_SOCKET;
    cmsg.cmsghdr_.cmsg_type = SCM_RIGHTS;
    cmsg.cmsghdr_.cmsg_len = CMSG_LEN(sizeof(fds));
    memcpy(CMSG_DATA(&cmsg.cmsghdr_), fds, sizeof(fds));

    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = &cmsg.cmsghdr_;
    msg.msg_controllen = sizeof(cmsg);

    ssize_t r;

    r = ::sendmsg(socket, &msg, 0);

    if (r < 0) {

//        check EAGAIN

    }

    std::cout << "sendmsg, fd: " << fds[0] << std::endl;
}

void
ipc_recv(socket_t socket, channel_t *channel, size_t size)  // receive fd from socket
{
    static constexpr int NUM_FD = 1;

    int fds[NUM_FD];  /* Contains the file descriptors to pass */

    struct iovec iov = {
            .iov_base = channel,
            .iov_len = size
    };
    union {         /* Ancillary data buffer, wrapped in a union
                              in order to ensure it is suitably aligned */
        struct ::cmsghdr cmsghdr_;
        char buf_[CMSG_SPACE(sizeof(fds))];

    } cmsg{};

    struct msghdr msg{};
    bzero(&msg, sizeof(msg));

    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = &cmsg.cmsghdr_;
    msg.msg_controllen = sizeof(cmsg);

    ssize_t r;

    r = recvmsg(socket, &msg, 0);

    if (r < 0) {
        throw ipc_recvmsg_failed_error();
    }

    if (r == 0) {
        throw ipc_recvmsg_zero_data_error();
    }

    if (r < size) {
        throw ipc_recvmsg_not_enough_data_error();
    }

    memcpy(&fds[0], CMSG_DATA(&cmsg.cmsghdr_), sizeof(fds));

    std::cout << "recvmsg, fd: " << fds[0] << std::endl;
}

socket_t
prepare_listen_socket(uint16 port, int *reuse_addr) {

    int r;
    r = ::socket(AF_INET, SOCK_STREAM, 0);

    if (r < 0) {
        throw socket_creation_failed_error();
    }

    socket_t sfd = r;

    if (reuse_addr) {

        r = ::setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (const void *) reuse_addr, sizeof(int));

        if (r < 0) {
            throw socket_reuse_opt_failed_error();
        }
    }

    set_nonblocking(sfd);

    struct ::sockaddr_in serv_addr = {0};

    ::bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    r = ::bind(sfd, (::sockaddr const *) &serv_addr, sizeof(serv_addr));

    if (r < 0) {
        throw socket_bind_failed_error();
    }

    r = ::listen(sfd, 5);

    if (r < 0) {
        throw socket_listen_failed_error();
    }

    return sfd;
}

}
}
