//
// Created by jonsl on 07/05/18.
//

#ifndef VIA_SOCKET_PAIR_H
#define VIA_SOCKET_PAIR_H

#include "sockets.h"
#include "error.h"

namespace via {

struct socket_pair {
    explicit
    socket_pair(int sv[2])
            : sv_{sv[0], sv[1]} {

        int r;

        r = sck_create_socketpair(sv_);
        if (r < 0) {
            throw socket_pair_creation_failed_error();
        }
    }

    ~socket_pair() {
        ::close(sv_[0]);
        ::close(sv_[1]);
    }

    int sv_[2]; // master writes to 1
};

}

#endif //VIA_SOCKET_PAIR_H
