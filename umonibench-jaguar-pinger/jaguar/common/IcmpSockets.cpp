//
// Created by blackwood on 12/9/16.
//

#include <jaguar/common/IcmpSockets.h>
#include <netinet/in.h>
#include <string.h>
#include <muduo/base/Logging.h>
#include <errno.h>
#include <arpa/inet.h>


namespace common {


    int IcmpSockets::icmpSocketGenerator(int sendBuf, int recvBuf) {

        int sockfd, on = 1;


        if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1) {
            LOG_ERROR << "ErrorNum=" << errno << " ErrorCode=" << strerror(errno) << " Msg=" <<
            "CreateRecvSockFdFailed";
            exit(EXIT_FAILURE);
        }


        // Providing IP Headers
        if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, (const char *) &on, sizeof(on)) == -1) {
            LOG_ERROR << "Unable to set IP_HDRINCL socket option";
            exit(EXIT_FAILURE);
        }


        if (setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (const char *) &recvBuf, sizeof(int)) < 0) {
            LOG_ERROR << "ErrorNum=" << errno << " ErrorCode=" << strerror(errno) << " Msg=" <<
            "SetReceiveBufTo65535Failed";
            exit(EXIT_FAILURE);
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (const char *) &sendBuf, sizeof(int)) < 0) {
            LOG_ERROR << "ErrorNum=" << errno << " ErrorCode=" << strerror(errno) << "Msg=" <<
            "SetSendBufTo65535Failed";
            exit(EXIT_FAILURE);
        }

        long size;
        socklen_t len = sizeof(long);
        getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char *) &size, &len);
        LOG_INFO << "Code=" << "SO_SNDBUF" << " SockFd=" << sockfd << " size=" << size;
        getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (char *) &size, &len);
        return sockfd;
    }


    int IcmpSockets::GetRecvIcmpSocket() {
        int sockfd = icmpSocketGenerator(200, 60 * 1024 * 1024);
        BindIcmpSockFd(sockfd);
        return sockfd;
    }

    int IcmpSockets::GetSenderIcmpSocket() {
        return icmpSocketGenerator(60 * 1024 * 1024, 200);
    }


    int IcmpSockets::BindIcmpSockFd(int sock_fd) {
        struct sockaddr_in servaddr;
        // Initializing servaddr to bind to all interfaces
        memset(&servaddr, 0, sizeof(struct sockaddr_in));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        if (bind(sock_fd, (struct sockaddr *) &servaddr, sizeof(struct sockaddr_in)) == -1) {
            LOG_ERROR << "Unable to bind";
            exit(EXIT_FAILURE);
        }
    }


    int32_t IcmpSockets::getInt32(char *m_PkgBuf, int parserIndex) {

        int32_t tmp = ((int32_t) m_PkgBuf[parserIndex] & 0xff) << 24 |
                      ((int32_t) m_PkgBuf[parserIndex + 1] & 0xff) << 16 |
                      ((int32_t) m_PkgBuf[parserIndex + 2] & 0xff) << 8 |
                      (int32_t) m_PkgBuf[parserIndex + 3] & 0xff;
        return tmp;
    }


    int32_t IcmpSockets::peekInt32From(size_t index, muduo::net::Buffer *pBuffer) {
        int32_t be32 = 0;

        ::memcpy(&be32, pBuffer->peek() + index, sizeof be32);
        return muduo::net::sockets::networkToHost32(be32);
    }

    int8_t IcmpSockets::peekInt8From(size_t index, muduo::net::Buffer *pBuffer) {
        assert(pBuffer->readableBytes() - index >= sizeof(int8_t));
        int8_t x = *(pBuffer->peek() + index);
        return x;
    }


    int64_t IcmpSockets::getInt64(char *memory, int index) {
        return ((int64_t) memory[index] & 0xff) << 56 |
               ((int64_t) memory[index + 1] & 0xff) << 48 |
               ((int64_t) memory[index + 2] & 0xff) << 40 |
               ((int64_t) memory[index + 3] & 0xff) << 32 |
               ((int64_t) memory[index + 4] & 0xff) << 24 |
               ((int64_t) memory[index + 5] & 0xff) << 16 |
               ((int64_t) memory[index + 6] & 0xff) << 8 |
               (int64_t) memory[index + 7] & 0xff;
    }


    void  IcmpSockets::setInt16(char *buf, int index, int16_t value) {
        buf[index] = ((uint16_t) value >> 8);
        buf[index + 1] = (uint16_t) value;
    }

    void  IcmpSockets::setInt32(char *buf, int index, int32_t value) {

        buf[index] = ((uint32_t) value >> 24);
        buf[index + 1] = ((uint32_t) value >> 16);
        buf[index + 2] = ((uint32_t) value >> 8);
        buf[index + 3] = (uint32_t) value;
    }

    void  IcmpSockets::setInt64(char *buf, int index, int64_t value) {

        buf[index] = ((uint64_t) value >> 56);

        buf[index + 1] = ((uint64_t) value >> 48);
        buf[index + 2] = ((uint64_t) value >> 40);
        buf[index + 3] = ((uint64_t) value >> 32);
        buf[index + 4] = ((uint64_t) value >> 24);
        buf[index + 5] = ((uint64_t) value >> 16);
        buf[index + 6] = ((uint64_t) value >> 8);
        buf[index + 7] = (uint64_t) value;
    }


    void IcmpSockets::ipToUint32(const char *ipStr, uint32_t &ipUint32) {
        ipUint32 = inet_addr(ipStr);
    }


    std::string IcmpSockets::uint32ToIp(uint32_t ipUint32) {
        in_addr addr;
        addr.s_addr = ipUint32;
        std::string ipStr = inet_ntoa(addr);
        return ipStr;
    }


}
