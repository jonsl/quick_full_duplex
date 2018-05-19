//
// Created by jslater on 17/04/18.
//

#ifndef VIA_WORKER_H
#define VIA_WORKER_H

#include "config.h"
#include "error.h"
#include "process.h"

namespace via {

static constexpr ssize_t RECV_BUFFER_SIZE = 4096;

class _VIA_PUBLIC worker : public process {

public:

    worker()
            : process("worker_"), buf_(RECV_BUFFER_SIZE) {

    }

    ~worker() override = default;

    void child_run() override {

        sigset_t set{};

        sigemptyset(&set);

        int r;

        r = sigprocmask(SIG_SETMASK, &set, nullptr);
        if (r < 0) {

        }

        time_msec_t timeout(0);

        std::thread::id this_id = std::this_thread::get_id();
        std::cout << "starting reactor on worker: " << this_id << std::endl;

        long loop_counter = 0;

        int min_wait_time = 1;


        ssize_t size;

        while ((size = ::read(sv_[CHILD_SOCKET_INDEX], &buf_[0], RECV_BUFFER_SIZE)) > 0) {

            sprintf(stdout, "worker::read", );

            ::write(STDOUT_FILENO, &buf_[0], (size_t) size);

//            wall_clock::update();
//            time_msec_t next_timeout = timer_queue_.expire_timers();
//
//            if (next_timeout != invalid_time_msec) {
//                timeout = next_timeout;
//            }
//
//            if (timeout < min_wait_time) {
//                timeout = min_wait_time;
//            }
//
//            reactor_.handle_events(timeout);

            ++loop_counter;
        }

    }

private:

    std::vector<char> buf_;
};

}

#endif //VIA_WORKER_H
