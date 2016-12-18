//
// Created by blackwood on 12/9/16.
//

#ifndef UMONIBENCH_JAGUAR_PING_PINGEXECUTOR_H
#define UMONIBENCH_JAGUAR_PING_PINGEXECUTOR_H

#include <jaguar/common/PingEntry.h>
using namespace common;

namespace core {

    class PingExecutor {


    public:

        explicit PingExecutor() : isRunning(true) {
            setUp();
        };


        ~PingExecutor();

        void setUp();

        void doExecution();


        void pingTask(PingEntryPtr pingEntryPtr);


    private:


        int sockfd;
        bool isRunning;
    };


}

#endif //UMONIBENCH_JAGUAR_PING_PINGEXECUTOR_H
