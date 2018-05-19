//
// Created by jonsl on 07/05/18.
//

#ifndef VIA_PROCESS_H
#define VIA_PROCESS_H

#include "sockets.h"
#include "error.h"

namespace via {

struct process {

    enum socket_index {
        NULL_SOCKET_INDEX = -1, CHILD_SOCKET_INDEX = 0, PARENT_SOCKET_INDEX = 1
    };

    explicit
    process(std::string name)
            : name_(std::move(name)), parent_pid_(getppid()), this_pid_(getpid()), sv_{INVALID_FD, INVALID_FD} {

        int r;

        r = sck_create_socketpair(sv_);
        if (r < 0) {
            throw process_socket_pair_creation_failed_error();
        }
    }

    virtual ~process() {
        ::close(sv_[CHILD_SOCKET_INDEX]);
        ::close(sv_[PARENT_SOCKET_INDEX]);
    }

    pid_t fork() {

        pid_t pid = ::fork();

        if (pid == -1) {
            // fork error
            std::cerr << "process::fork error: " << ::strerror(errno) << std::endl;
            throw process_fork_failure_error();

        } else if (pid) {
            // in parent process
            ::close(sv_[CHILD_SOCKET_INDEX]);

            std::cout << "parent: (parent, child) =  (" << parent_pid_ << ", " << this_pid_ << ")" << std::endl;

            run_parent();

        } else {
            // in child process
            ::close(sv_[PARENT_SOCKET_INDEX]);

            parent_pid_ = this_pid_;
            this_pid_ = getpid();
            std::cout << "child: (parent, child) =  (" << parent_pid_ << ", " << this_pid_ << ")" << std::endl;

            run_child();
        }

//        ngx_log_error(NGX_LOG_NOTICE, cycle->log, 0, "start %s %P", name, pid);

//        ngx_processes[s].pid = pid;
//        ngx_processes[s].exited = 0;

        return pid;
    }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    void run_child() {

        // command signals

        ssize_t r;

        for (;;) {

            procinfo_t procinfo{};

            r = sck_ipc_recv(sv_[CHILD_SOCKET_INDEX], &procinfo, sizeof(procinfo));

            if (r == sizeof(procinfo)) {

                // ok

                std::cout << "child received ok: procinfo={" << procinfo.command_ << ", " << procinfo.pid_ << ", "
                          << procinfo.slot_ << ", " << procinfo.fd_ << ", " << procinfo.type_ << "}" << std::endl;

            } else {

//                std::cout << "not received msg, r is " << r << std::endl;

//                break;
            }

        }

        // socket data


    }
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    void run_parent() {

        ssize_t r;

        for (;;) {

            procinfo_t procinfo{};
            std::cout << "command: ";
            std::cin >> procinfo.command_;

            r = sck_ipc_send(sv_[PARENT_SOCKET_INDEX], &procinfo, sizeof(procinfo));

            if (r == sizeof(procinfo)) {

                // ok
                std::cout << "parent sent ok, r: " << r << std::endl;

            } else {

//                std::cout << "not received msg, r is " << r << std::endl;

//                break;
            }

        }


    }
#pragma clang diagnostic pop


    std::string name_;

    pid_t parent_pid_;
    pid_t this_pid_;

    fd_t sv_[2];
};

}

#endif //VIA_PROCESS_H
