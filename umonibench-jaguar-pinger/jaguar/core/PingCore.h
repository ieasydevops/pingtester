//
// Created by blackwood on 12/2/16.
//

#ifndef UMONIBENCH_JAGUAR_PING_PINGCORE_H
#define UMONIBENCH_JAGUAR_PING_PINGCORE_H

#include <stddef.h>
#include <muduo/base/LogFile.h>


using namespace muduo;


namespace core {


    static const int kRollSize = 1 * 1000;


    class PingCore {

    public:
        static int start(int argc, char **argv);

        static int stop();

        static int processArgs(int argc, char **argv);

    private:

    };


}


#endif //UMONIBENCH_JAGUAR_PING_PINGCORE_H
