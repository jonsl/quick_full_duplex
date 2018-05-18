//
// Created by jslater on 17/04/18.
//

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include "sockets.h"

int
sck_set_nonblocking(fd_t sfd) {

    int r;

    r = fcntl(sfd, F_GETFL);
    if (r < 0) {
        fprintf(stderr, "sck_set_nonblocking F_GETFL error: %s\n", strerror(errno));
        return r;
    }

    r = fcntl(sfd, F_SETFL, r | O_NONBLOCK);
    if (r < 0) {
        fprintf(stderr, "sck_set_nonblocking F_SETFL error: %s\n", strerror(errno));
        return r;
    }

    return r;
}

int
sck_create_socketpair(fd_t sfds[2]) {

    int r;

    r = socketpair(AF_UNIX, SOCK_STREAM, 0, &sfds[0]);
    if (r < 0) {
        fprintf(stderr, "sck_create_socketpair error: %s\n", strerror(errno));
        return r;
    }

    r = sck_set_nonblocking(sfds[0]);
    if (r < 0) {
        return r;
    }

    r = sck_set_nonblocking(sfds[1]);
    if (r < 0) {
        return r;
    }

    return r;
}

ssize_t
sck_ipc_send(fd_t sfd, struct procinfo_t *procinfo, size_t size) {

    static const int NUM_FD = 1;

    int fds[NUM_FD];  /* Contains the file descriptors to pass */

    struct iovec iov = {
            .iov_base = procinfo,
            .iov_len = size
    };

    union {         /* Ancillary data buffer, wrapped in a union
                              in order to ensure it is suitably aligned */
        struct cmsghdr cmsghdr_;
        char buf_[CMSG_SPACE(sizeof(fds))];

    } cmsg;
    bzero(&cmsg, sizeof(cmsg));

    cmsg.cmsghdr_.cmsg_level = SOL_SOCKET;
    cmsg.cmsghdr_.cmsg_type = SCM_RIGHTS;
    cmsg.cmsghdr_.cmsg_len = CMSG_LEN(sizeof(fds));
    memcpy(CMSG_DATA(&cmsg.cmsghdr_), fds, sizeof(fds));

    struct msghdr msg;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = &cmsg.cmsghdr_;
    msg.msg_controllen = sizeof(cmsg);

    ssize_t r;

    r = sendmsg(sfd, &msg, 0);
    if (r < 0) {
        fprintf(stderr, "sck_ipc_send sendmsg error: %s\n", strerror(errno));
        return r;
    }

    fprintf(stdout, "sck_ipc_send sendmsg, fds: %i\n", fds[0]);

    return r;
}

ssize_t
sck_ipc_recv(fd_t sfd, struct procinfo_t *procinfo, size_t size) { // receive fd from server_socket

    static const int NUM_FD = 1;

    fd_t fds[NUM_FD];  /* Contains the file descriptors to pass */

    struct iovec iov = {
            .iov_base = procinfo,
            .iov_len = size
    };
    union {         /* Ancillary data buffer, wrapped in a union
                              in order to ensure it is suitably aligned */
        struct cmsghdr cmsghdr_;
        char buf_[CMSG_SPACE(sizeof(fds))];

    } cmsg;
    bzero(&cmsg, sizeof(cmsg));

    struct msghdr msg;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = &cmsg.cmsghdr_;
    msg.msg_controllen = sizeof(cmsg);

    ssize_t r;

    r = recvmsg(sfd, &msg, 0);
    if (r < 0) {
        fprintf(stderr, "sck_ipc_recv recvmsg (r < 0) error: %s\n", strerror(errno));
        return r;
    }

    if (r == 0) {
        fprintf(stderr, "sck_ipc_recv recvmsg (r == 0) error: %s\n", strerror(errno));
        return r;
    }

    if (r < size) {
        fprintf(stderr, "sck_ipc_recv recvmsg (r < size) error: %s\n", strerror(errno));
        return r;
    }

    memcpy(&fds[0], CMSG_DATA(&cmsg.cmsghdr_), sizeof(fds));

    fprintf(stdout, "sck_ipc_recv recvmsg, fds: %i\n", fds[0]);

    return 0;
}

fd_t
sck_create_socket(int domain) {
    assert(domain == AF_INET || domain == AF_INET6);

    int r;

    r = socket(domain, SOCK_STREAM, 0);
    if (r < 0) {
        fprintf(stderr, "sck_create_socket socket error: %s\n", strerror(errno));
        return r;
    }

    fd_t sfd = r;
    assert(sfd != INVALID_FD);

    return sfd;
}

/// bind socket
/// \param sfd valid socket file descriptor
/// \param af family type, must be AF_INET or AF_INET6
/// \param host address of host or nullptr to bind socket to any process
/// \param port desired port or 0 for os to assign a port
/// \param port_out set to port actually used
/// \return < 0 for error or 0 for success
int
sck_bind_socket(fd_t sfd, sa_family_t af, char const *host, uint16 port, uint16 *port_out) {

    assert(sfd != INVALID_FD);
    assert(af == AF_INET || af == AF_INET6);
    assert(port_out);

    int r;

    struct sockaddr_in6 sa;
    bzero(&sa, sizeof(sa));

    sa.sin6_family = af;
    sa.sin6_port = htons(port);

// store ip address in sa
    if (host) {
        r = inet_pton(af, host, &sa.sin6_addr);
        if (r <= 0) {
            fprintf(stderr, "sck_bind_socket inet_pton error: %s\n", strerror(errno));
            return r;
        }
    } else {
        sa.sin6_addr = in6addr_any; // os to assign a port
    }

    r = bind(sfd, (struct sockaddr const *) &sa, sizeof(sa));
    if (r < 0) {
        fprintf(stderr, "sck_bind_socket bind error: %s\n", strerror(errno));
        return r;
    }

// retrieve ip address in sa
    char dst[INET6_ADDRSTRLEN];

    if (inet_ntop(af, &sa.sin6_addr, dst, INET6_ADDRSTRLEN) == NULL) {
        fprintf(stderr, "sck_bind_socket inet_ntop error: %s\n", strerror(errno));
        return -1;
    }

    fprintf(stdout, "sck_bind_socket retrieved ip address: %s\n", dst);

// update port number
    struct sockaddr_in6 sao;
    bzero(&sao, sizeof(sao));
    socklen_t sao_len = sizeof(sao);

    r = getsockname(sfd, (struct sockaddr *) &sao, &sao_len);
    if (r < 0) {
        fprintf(stderr, "sck_bind_socket getsockname error: %s\n", strerror(errno));
        return r;
    }

    *port_out = (uint16) ntohs(sao.sin6_port);

    fprintf(stdout, "sck_bind_socket retrieved port: %i\n", *port_out);
    return r;
}

int
sck_listen(fd_t sfd) {

    assert(sfd != INVALID_FD);

    int r;

    r = listen(sfd, SOMAXCONN);
    if (r < 0) {
        fprintf(stderr, "sck_listen listen error: %s\n", strerror(errno));
        return r;
    }

    return r;
}
