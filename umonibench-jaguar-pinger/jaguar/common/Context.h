//
// Created by blackwood on 12/11/16.
//

#ifndef UMONIBENCH_JAGUAR_PINGER_CONTEXT_H
#define UMONIBENCH_JAGUAR_PINGER_CONTEXT_H

#include <stdint-gcc.h>
#include <muduo/base/Mutex.h>
#include <jaguar/common/Singleton.h>

namespace common {


    class Context {
    public:
        Context();

        ~Context();

        uint32_t getPid();

        const char *getHost() {
            return "192.168.1.101";
            return "10.100.21.13";
        };


    private:

        uint16_t _pid;
    };


    typedef Singleton<Context, muduo::MutexLock> _gCtx;
}


#endif //UMONIBENCH_JAGUAR_PINGER_CONTEXT_H
