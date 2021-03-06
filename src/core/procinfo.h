//
// Created by jonsl on 07/05/18.
//

#ifndef PROCINFO_H
#define PROCINFO_H

#include "config.h"
#include "proctype.h"

struct procinfo_t {
    uint32 command_;
    pid_t owner_;
    enum proctype_t type_;
};

#endif //PROCINFO_H
