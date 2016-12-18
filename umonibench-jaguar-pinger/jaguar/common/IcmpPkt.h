//
// Created by blackwood on 12/9/16.
//

#ifndef UMONIBENCH_JAGUAR_PING_ICMPPKT_H
#define UMONIBENCH_JAGUAR_PING_ICMPPKT_H

#include <stdint.h>
#include <stddef.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <muduo/net/Buffer.h>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <stdio.h>
#include <muduo/base/Timestamp.h>
#include <inttypes.h>
#include "IcmpSockets.h"


using namespace muduo;

namespace common {


    struct IcmpBody {
        uint32_t _ipInt;
        uint64_t _taskId;
        uint64_t _sendTime;
    public:

        int writeTo(char *pBuffer) {
            int index = 0;


            IcmpSockets::setInt32(pBuffer, index, muduo::net::sockets::hostToNetwork32(_ipInt));
            index += 4;

            IcmpSockets::setInt64(pBuffer, index, muduo::net::sockets::hostToNetwork64(_taskId));
            index += 8;


            IcmpSockets::setInt64(pBuffer, index, muduo::net::sockets::hostToNetwork64(
                    muduo::Timestamp::now().microSecondsSinceEpoch()));
            index += 8;

        };


        void readFrom(char *pBuffer) {
            int index = 0;
            this->_ipInt = muduo::net::sockets::networkToHost32(IcmpSockets::getInt32(pBuffer, index));
            index += 4;
            this->_taskId = muduo::net::sockets::networkToHost64(IcmpSockets::getInt64(pBuffer, index));
            index += 8;
            this->_sendTime = muduo::net::sockets::networkToHost64(IcmpSockets::getInt64(pBuffer, index));
            index += 8;
        }

    };


    const static int ICMP_MIN_LEN = 1  // type
                                    + 1  // code
                                    + 2   // chksum
                                    + 2  //id
                                    + 2  //seq
    ;

    const static int MAX_MTU = 1500;


    class IcmpPkt {


    public:


        IcmpPkt();


        ~IcmpPkt();


        IcmpPkt(int maxPayLoad);

        // build IcmpPkt  buffer for pingTarget
        void buildWithTask(uint32_t ipInt, uint16_t payLoadSize, uint64_t taskId, uint16_t seq);


        IcmpPkt(uint32_t ipInt, uint16_t pktSize, uint64_t taskId, uint16_t seq);


        int writeTo(int fd);


        int writeToSockBuf(muduo::net::Buffer *pBuffer);


        int buildAndCheck();


    public:

        // pBuffer used for receive msg form socket , and which  should be initialized with fixed size first;


        int64_t _recvTime;
        int64_t _rrt;
        int32_t _pktLen;
        char *pBuffer;
        IcmpBody _body;
    private:


        // icmpPkt header
        struct in_addr src_addr;
        struct in_addr dest_addr;

        bool _isSucess;
        int _type;
        uint16_t seq;


    private:


        int pktMiniSize();

        void buildHeader(struct iphdr *ip, struct icmphdr *icmp);

        uint16_t icmpChecksum(const char *data, int length);


    };


    typedef boost::shared_ptr<IcmpPkt> IcmpPktPrt;
}

#endif //UMONIBENCH_JAGUAR_PING_ICMPPKT_H
