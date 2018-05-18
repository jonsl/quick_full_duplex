//
// Created by jslater on 10/03/18.
//

#ifndef VIA_ERROR_H
#define VIA_ERROR_H

#include "config.h"

namespace via {

class _VIA_PUBLIC error : public std::logic_error {
public:
    explicit error(std::string const &what) : std::logic_error(what) {}
};

class _VIA_PUBLIC socket_pair_creation_failed_error : public error {
public:
    explicit socket_pair_creation_failed_error()
            : error("failed to create socketpair error") {}
};

class _VIA_PUBLIC could_not_create_socket_error : public error {
public:
/// The Base class for via errors.
    explicit could_not_create_socket_error()
            : error("master could not create a server_socket error") {}
};

class _VIA_PUBLIC could_not_listen_on_socket_error : public error {
public:
    explicit could_not_listen_on_socket_error()
            : error("master could not listen to a server_socket error") {}
};

class _VIA_PUBLIC master_could_not_create_socket_error : public error {
public:
    explicit master_could_not_create_socket_error()
            : error("master could not create a server_socket error") {}
};

class _VIA_PUBLIC master_could_not_accept_socket_error : public error {
public:
    explicit master_could_not_accept_socket_error()
            : error("master could not accept server_socket error") {}
};

class _VIA_PUBLIC server_socket_could_not_bind_socket_error : public error {
public:
    explicit server_socket_could_not_bind_socket_error()
            : error("bind server_socket invalid network address for the specified family error") {}
};

}

#endif //VIA_ERROR_H
