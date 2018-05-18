
#include "sockets.h"

int main() {

    int const nworkers = 1;

    int sv[2];

    int r;

    r = sck_create_socketpair(sv);
    if (r < 0) {
        return r;
    }

    sigset_t set{};

    for (int i = 0; i < nworkers; ++i) {

        int pid;

        pid = fork();

        if (pid == 0) {

            // child process

//            via::worker<via::via_list_arena> worker(handlers, socket_pair, allocator_1);

//            worker.run();

        } else {

//            via::master<via::via_list_arena> master(socket_pair, allocator_1);

//            close(sv[0]); // close the read-end of the pipe, I'm not going to use it
//            write(sv[1], "hello worldsscsacascsacascasascascsacssasc", strlen("hello worldscascsacsacascsaccascsas")); // send the content of argv[1] to the reader
//            close(sv[1]); // close the write-end of the pipe, thus sending EOF to the reader
//            wait(nullptr); // wait for the child process to exit before I do the same
        }

    }



    std::cout << "Hello, World!" << std::endl;
    return 0;
}
