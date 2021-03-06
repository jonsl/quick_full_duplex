//
// Created by jonsl on 07/05/18.
//

#ifndef VIA_PROCESS_H
#define VIA_PROCESS_H

#include "sockets.h"
#include "error.h"

namespace via {

class process {

public:

    enum socket_index {
        CHILD_SOCKET_INDEX = 0, PARENT_SOCKET_INDEX = 1
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

            std::cout << "parent: (parent, this) =  (" << parent_pid_ << ", " << this_pid_ << ")" << std::endl
                      << std::flush;

            run_parent();

        } else {
            // in child process
            ::close(sv_[PARENT_SOCKET_INDEX]);

            parent_pid_ = this_pid_;
            this_pid_ = getpid();
            std::cout << "child: (parent, this) =  (" << parent_pid_ << ", " << this_pid_ << ")" << std::endl
                      << std::flush;

            run_child();
        }

        return pid;
    }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

    void run_child() {

        for (;;) {

            procinfo_t procinfo{};

            static constexpr size_t NUM_FDS = 2;
            fd_t fds[NUM_FDS];

            ssize_t r;

            r = sck_ipc_recv(sv_[CHILD_SOCKET_INDEX], &procinfo, sizeof(procinfo), fds, NUM_FDS);

            if (r == sizeof(procinfo)) {

                // received whole procinfo
                std::cout << "child " << r << " bytes received ok => procinfo = {"
                          << procinfo.command_ << "," << procinfo.owner_ << "," << procinfo.type_ << "}" << std::endl;

                for (fd_t fd : fds) {
                    fprintf(stdout, "child fd: %i contents:\n", fd);
                    write_fd(fd);

                    ::close(fd);
                }
                std::cout << std::endl;

            } else {


            }

        }

    }

#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

    void run_parent() {

        for (;;) {

            procinfo_t procinfo{};
            std::cout << "\ncommand: ";
            std::cin >> procinfo.command_;
            procinfo.owner_ = this_pid_;
            procinfo.type_ = MASTER_PROCTYPE;

            static constexpr size_t NUM_FDS = 2;

            fd_t fd_1 = ::open("file-1.txt", O_RDONLY | O_CREAT, 0666);
            fd_t fd_2 = ::open("file-2.txt", O_RDONLY | O_CREAT, 0666);

            fd_t fds[NUM_FDS] = {

                    fd_1,
                    fd_2

            };

            std::cout << "parent sending: fd_1: " << fd_1 << ", fd_2: " << fd_2 << std::endl;

            ssize_t r;

            r = sck_ipc_send(sv_[PARENT_SOCKET_INDEX], &procinfo, sizeof(procinfo), fds, NUM_FDS);

            ::close(fd_2);
            ::close(fd_1);

            if (r == sizeof(procinfo)) {

                // ok
                std::cout << "parent " << r << " bytes sent ok" << std::endl;

            } else {

            }
        }
    }

#pragma clang diagnostic pop

private:

    void write_fd(fd_t fd) {

        static constexpr int RECV_BUFF_SIZE = 1024;
        std::vector<char> buf(RECV_BUFF_SIZE);

        ssize_t r;

        while ((r = ::read(fd, &buf[0], RECV_BUFF_SIZE)) > 0) {
            ::write(STDOUT_FILENO, &buf[0], r);
        }
    }

private:

    std::string name_;

    pid_t parent_pid_;
    pid_t this_pid_;

    fd_t sv_[2];
};

}

#endif //VIA_PROCESS_H
