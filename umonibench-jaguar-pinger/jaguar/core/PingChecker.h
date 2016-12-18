//
// Created by blackwood on 12/11/16.
//

#ifndef UMONIBENCH_JAGUAR_PINGER_PINGCHECKER_H
#define UMONIBENCH_JAGUAR_PINGER_PINGCHECKER_H

#include <boost/noncopyable.hpp>



namespace core {

    class PingChecker {
    public:
        explicit PingChecker()
                : isRunning(true) {
        }

        ~PingChecker();

        void doChecker();

    private:
        bool isRunning;
    };


}


#endif //UMONIBENCH_JAGUAR_PINGER_PINGCHECKER_H
