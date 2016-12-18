//
// Created by blackwood on 12/3/16.
//

#ifndef UMONIBENCH_JAGUAR_PING_PINGTASKGEN_H
#define UMONIBENCH_JAGUAR_PING_PINGTASKGEN_H


#include <muduo/base/Atomic.h>

#include <set>


using namespace muduo;


namespace core {

    class PingTaskGen {

    public:


    public:

        explicit PingTaskGen(const char *fileName)
                : isRunning(true), pfileName(fileName) {

        };


        ~PingTaskGen();

        void doTaskGen();


    private:
        void loadPingTaskFromFile(const char *fileName);
        bool isRunning;
        const char *const pfileName;
        AtomicInt64 _taskId;
    };


}

#endif //UMONIBENCH_JAGUAR_PING_PINGTASKGEN_H
