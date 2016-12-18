//
// Created by blackwood on 12/9/16.
//

#include <jaguar/core/PingReceiver.h>

#include <unistd.h>
#include <jaguar/common/IcmpPkt.h>
#include <muduo/base/Logging.h>
#include <errno.h>
#include <jaguar/common/PingQue.h>


using namespace common;

namespace core {


    PingReceiver::~PingReceiver() {
        close(sockfd);
    }


    void PingReceiver::doReceiver() {

        this->sockfd = common::IcmpSockets::GetRecvIcmpSocket();

        while (isRunning) {

            IcmpPktPrt icmpPktPrt(new IcmpPkt(MAX_MTU));
            int n = recvfrom(sockfd, icmpPktPrt->pBuffer, MAX_MTU, 0, pRecvAddr, pRecvAddr_len);

            if (n >= 0) {
                icmpPktPrt->_pktLen = n;
                icmpPktPrt->_recvTime = muduo::Timestamp::now().microSecondsSinceEpoch();
                SingletonIcmpPktQ::Instance()->recvIcmpPktQ.put(icmpPktPrt);
                LOG_INFO << "RecvOneIcmpPkt " << " readLen=" << n << " recvIcmpPktQSize=" <<
                         SingletonIcmpPktQ::Instance()->recvIcmpPktQ.size() << " time=" << icmpPktPrt->_recvTime;

            } else {

                LOG_ERROR << " ErrorNum=" << errno << " ErrorCode=" << strerror(errno) << " MSG=" <<
                "ReceiveIcmpError";
            }

        }
    }


}