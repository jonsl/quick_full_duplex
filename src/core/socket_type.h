//
// Created by jonsl on 12/05/18.
//

#ifndef VIA_SOCKET_TYPE_H
#define VIA_SOCKET_TYPE_H

#include "config.h"

namespace via {

enum class socket_type : uint32 {

    NULL_SOCKET_TYPE = 0,

    CLIENT,

    SERVER
};

}

#endif //VIA_SOCKET_TYPE_H
