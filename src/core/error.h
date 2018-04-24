//
// Created by jslater on 10/03/18.
//

#ifndef VIA_ERROR_H
#define VIA_ERROR_H

#include "config.h"

namespace via {

/// The Base class for via errors.
class _VIA_PUBLIC error : public std::logic_error {
public:
    explicit error(std::string const &what) : std::logic_error(what) {}
};

class _VIA_PUBLIC socket_set_nonblocking_failed_error : public error {
public:
    explicit socket_set_nonblocking_failed_error()
            : error("socket set non-blocking failed error") {}
};

class _VIA_PUBLIC ipc_socketpair_creation_failed_error : public error {
public:
    explicit ipc_socketpair_creation_failed_error()
            : error("IPC socketpair creation failed error") {}
};

class _VIA_PUBLIC ipc_recvmsg_failed_error : public error {
public:
    explicit ipc_recvmsg_failed_error()
            : error("IPC recvmsg failed error") {}
};

class _VIA_PUBLIC ipc_recvmsg_zero_data_error : public error {
public:
    explicit ipc_recvmsg_zero_data_error()
            : error("IPC recvmsg zero data error") {}
};

class _VIA_PUBLIC ipc_recvmsg_not_enough_data_error : public error {
public:
    explicit ipc_recvmsg_not_enough_data_error()
            : error("IPC recvmsg not enough data error") {}
};

class _VIA_PUBLIC socket_creation_failed_error : public error {
public:
    explicit socket_creation_failed_error()
            : error("listen socket creation failed error") {}
};

class _VIA_PUBLIC socket_reuse_opt_failed_error : public error {
public:
    explicit socket_reuse_opt_failed_error()
            : error("socket reuse option failed error") {}
};

class _VIA_PUBLIC socket_bind_failed_error : public error {
public:
    explicit socket_bind_failed_error()
            : error("socket bind failed error") {}
};

class _VIA_PUBLIC socket_listen_failed_error : public error {
public:
    explicit socket_listen_failed_error()
            : error("socket listen failed error") {}
};

}

#endif //VIA_ERROR_H
