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
sck_ipc_send(fd_t sfd, struct procinfo_t *procinfo, size_t size, fd_t *fds, size_t nfds) {

    assert(procinfo);

    struct iovec iov[1];
    iov[0].iov_base = procinfo;
    iov[0].iov_len = size;
    union {         /* Ancillary data buffer, wrapped in a union
                              in order to ensure it is suitably aligned */
        char buf_[CMSG_SPACE(sizeof(fd_t) * nfds)];
        struct cmsghdr cmsghdr_;
    } u;

    struct msghdr msg;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    msg.msg_control = u.buf_;
    msg.msg_controllen = sizeof(u.buf_);
    msg.msg_flags = 0;

    struct cmsghdr *cmsg;
    cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(fd_t) * nfds);
    fd_t *fdptr = (fd_t *) CMSG_DATA(cmsg);    /* Initialize the payload */
    bcopy(fds, fdptr, sizeof(fd_t) * nfds);

    ssize_t r;

    r = sendmsg(sfd, &msg, 0);

    if (r == -1) {
        if (errno == EAGAIN) {
            return errno;
        }
        fprintf(stderr, "sck_ipc_send sendmsg error: %s\n", strerror(errno));
        return r;
    }

    return r;
}

/// receive fd from server_socket
/// \param sfd
/// \param procinfo
/// \param size
/// \param fds
/// \param nfds
/// \return
ssize_t
sck_ipc_recv(fd_t sfd, struct procinfo_t *procinfo, size_t size, fd_t *fds, size_t nfds) {

    assert(procinfo);

    struct iovec iov[1];
    iov[0].iov_base = procinfo;
    iov[0].iov_len = size;
    union {         /* Ancillary data buffer, wrapped in a union
                              in order to ensure it is suitably aligned */
        char buf_[CMSG_SPACE(sizeof(fd_t) * nfds)];
        struct cmsghdr cmsghdr_;
    } u;

    struct cmsghdr *cmsghdr;
    cmsghdr = (struct cmsghdr *) u.buf_;
    cmsghdr->cmsg_len = CMSG_LEN(sizeof(fd_t) * nfds);
    cmsghdr->cmsg_level = SOL_SOCKET;
    cmsghdr->cmsg_type = SCM_RIGHTS;

    struct msghdr msg;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = iov;
    msg.msg_iovlen = sizeof(iov) / sizeof(iov[0]);
    msg.msg_control = u.buf_;
    msg.msg_controllen = sizeof(u.buf_);
    msg.msg_flags = 0;

    ssize_t r;

    r = recvmsg(sfd, &msg, 0);

    if (r == -1) {
        if (errno == EAGAIN) {
            return errno;
        }
        fprintf(stderr, "sck_ipc_recv recvmsg (r == -1) error: %s\n", strerror(errno));
        return r;
    }

    if (r < size) {
        fprintf(stderr, "sck_ipc_recv recvmsg (r < size) error: %ld\n", size);
        return -1;
    }

    fd_t *p = (fd_t *) CMSG_DATA(cmsghdr);
    bcopy(p, fds, sizeof(fd_t) * nfds);

    return r;
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
