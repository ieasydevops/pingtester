//
// Created by blackwood on 12/13/16.
//

#ifndef UMONIBENCH_JAGUAR_PINGER_PINGREPORTER_H
#define UMONIBENCH_JAGUAR_PINGER_PINGREPORTER_H


#include <jaguar/core/JaguarPingerServer.h>
#include <jaguar/common/PingTaskLifeCycleManager.h>


using namespace muduo;


namespace core {

    class PingReporter {
    public:

        PingReporter(JaguarPingerServer &server) : _server(server) {

        }



        void onPingResponseCompleted(PingResponsePtr &pingResPtr);


    private:

        JaguarPingerServer &_server;
    };


}


#endif //UMONIBENCH_JAGUAR_PINGER_PINGREPORTER_H
