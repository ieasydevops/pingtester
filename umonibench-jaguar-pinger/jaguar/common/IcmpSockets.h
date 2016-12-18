//
// Created by blackwood on 12/9/16.
//

#ifndef UMONIBENCH_JAGUAR_PING_ICMPSOCKETS_H
#define UMONIBENCH_JAGUAR_PING_ICMPSOCKETS_H


#include <stdint.h>
#include <string>
#include <muduo/net/Buffer.h>

namespace common {

    class IcmpSockets {
    public:


        static int icmpSocketGenerator(int sendBuf, int recvBuf);

        static int GetSenderIcmpSocket();

        static int GetRecvIcmpSocket();

        static int BindIcmpSockFd(int sock_fd);


        static int32_t peekInt32From(size_t index, muduo::net::Buffer *pBuffer);

        static int8_t peekInt8From(size_t index, muduo::net::Buffer *pBuffer);


        static void setInt16(char *buf, int index, int16_t value);

        static void setInt32(char *buf, int index, int32_t value);

        static void setInt64(char *buf, int index, int64_t value);


        static int32_t getInt32(char *m_PkgBuf, int parserIndex);

        static int64_t getInt64(char *memory, int index);


        static void ipToUint32(const char *ipStr, uint32_t &ipUint32);


        static std::string uint32ToIp(uint32_t ipUint32);


    };


}


#endif //UMONIBENCH_JAGUAR_PING_ICMPSOCKETS_H
