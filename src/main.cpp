
#include <sstream>
#include "process.h"

std::vector<via::process> workers;

int main() {

    int const nprocessess = 1;


    workers.reserve(nprocessess);

    int sv[2];

    sigset_t set{};

    for (int i = 0; i < nprocessess; ++i) {

        via::process *pproc = &workers[i];

        std::stringstream ss;
        ss << "name_" << i;

        workers.get_allocator().construct(pproc, ss.str());

        pproc->fork();
    }

    //

    return 0;
}
