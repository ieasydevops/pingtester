//
// Created by blackwood on 12/9/16.
//

#ifndef UMONIBENCH_JAGUAR_PING_PINGRECEIVER_H
#define UMONIBENCH_JAGUAR_PING_PINGRECEIVER_H


#include <sys/socket.h>
#include <jaguar/common/IcmpSockets.h>

namespace core {


    class PingReceiver {
    public:

        explicit PingReceiver(): isRunning(true){

        };

        ~PingReceiver();

        void doReceiver();

    private:

        int sockfd;
        bool isRunning;
        struct sockaddr *pRecvAddr;
        socklen_t *pRecvAddr_len;


    };

}

#endif //UMONIBENCH_JAGUAR_PING_PINGRECEIVER_H
