//
// Created by blackwood on 12/9/16.
//

#include <jaguar/common/IcmpPkt.h>


#include <muduo/base/Timestamp.h>
#include <muduo/base/Logging.h>
#include <errno.h>
#include <jaguar/common/PingEntry.h>
#include <jaguar/common/Context.h>
#include <jaguar/common/IcmpSockets.h>
#include <arpa/inet.h>
#include <jaguar/common/Singleton.h>

namespace common {


    IcmpPkt::IcmpPkt() : pBuffer(NULL) {


    }

    IcmpPkt::~IcmpPkt() {

        LOG_DEBUG << " IcmpPkt Destory " << " taskId=" << _body._taskId << " ip=" << _body._ipInt;
        if (pBuffer != NULL) {
            delete[](pBuffer);
            pBuffer = NULL;
        }

    }

    IcmpPkt::IcmpPkt(int maxPktSize) {
        pBuffer = new char[maxPktSize];
    }


    int IcmpPkt::pktMiniSize() {
        return sizeof(struct iphdr) + sizeof(struct icmphdr) + sizeof(struct IcmpBody);
    }


    IcmpPkt::IcmpPkt(uint32_t ipInt, uint16_t pktSize, uint64_t taskId, uint16_t seq) : pBuffer(NULL) {
        _pktLen = pktSize > pktMiniSize() ? pktSize : pktMiniSize();


        this->_body._ipInt = ipInt;
        this->_body._taskId = taskId;

        _type = ICMP_ECHO;
        inet_pton(AF_INET, _gCtx::Instance()->getHost(), &src_addr);
        inet_pton(AF_INET, IcmpSockets::uint32ToIp(ipInt).c_str(), &dest_addr);

    }


    int IcmpPkt::writeTo(int fd) {
        struct sockaddr_in targetAddr;

        struct iphdr *ip;
        struct icmphdr *icmp;
        char *icmp_payload;


        pBuffer = new char[_pktLen];

        if (pBuffer == NULL) {
            LOG_ERROR << "No memory available";
            close(fd);
            exit(EXIT_FAILURE);
        }


        // Initializing header and payload pointers
        ip = (struct iphdr *) pBuffer;
        icmp = (struct icmphdr *) (pBuffer + sizeof(struct iphdr));
        icmp_payload = (char *) (pBuffer + sizeof(struct iphdr) + sizeof(struct icmphdr));
        buildHeader(ip, icmp);

        ip->tot_len = htons(_pktLen);
        ip->saddr = src_addr.s_addr;
        ip->daddr = dest_addr.s_addr;

        _body.writeTo(icmp_payload);

        LOG_DEBUG << " icmpPktWriteBodyDone" << "taskId=" <<
                  muduo::net::sockets::networkToHost64(IcmpSockets::getInt64(icmp_payload, 4)) << " time=" <<
                  muduo::net::sockets::networkToHost64(IcmpSockets::getInt64(icmp_payload, 12));


        icmp->type = _type;
        icmp->checksum = 0;
        int chLen = _pktLen - sizeof(struct iphdr);

        LOG_DEBUG << "chLen=" << chLen;
        icmp->checksum = icmpChecksum((char *) icmp, chLen);

        // Sending the packet
        struct sockaddr_in target;
        target.sin_family = AF_INET;
        target.sin_addr.s_addr = dest_addr.s_addr;
        return sendto(fd, pBuffer, _pktLen, 0, (struct sockaddr *) &target, sizeof(struct sockaddr_in));

    }


    int IcmpPkt::writeToSockBuf(muduo::net::Buffer *pBuffer) {
        pBuffer->appendInt64(_body._taskId);
        pBuffer->appendInt32(_body._ipInt);
        pBuffer->appendInt8(this->_isSucess ? 1 : 0);
        pBuffer->appendInt32(_rrt);
    }


    int IcmpPkt::buildAndCheck() {


        struct iphdr *ip;
        struct icmphdr *icmp;
        char *icmp_payload;

        ip = (struct iphdr *) pBuffer;
        icmp = (struct icmphdr *) (pBuffer + sizeof(struct iphdr));
        icmp_payload = (char *) (pBuffer + sizeof(struct iphdr) + sizeof(struct icmphdr));

        // Filling up packet_details
        src_addr.s_addr = ip->saddr;
        dest_addr.s_addr = ip->daddr;
        _type = icmp->type;
        _body.readFrom(icmp_payload);
        LOG_DEBUG << " ipHeader src=" << inet_ntoa(src_addr) << " dst=" << inet_ntoa(dest_addr) << " ipInt=" <<
                  _body._ipInt;
        if (icmp->type != ICMP_ECHOREPLY) {
            LOG_ERROR << " IcmpTypeInvalid " << " expect=" << ICMP_ECHOREPLY << " actual=" << icmp->type;
            return -1;
        }

        LOG_DEBUG << " CheckIcmpTypeOk" << " expect=" << ICMP_ECHOREPLY << " actual=" << icmp->type;


        uint16_t pid = muduo::net::sockets::networkToHost16(icmp->un.echo.id);


        uint16_t seq = muduo::net::sockets::networkToHost16(icmp->un.echo.sequence);

        if (pid != _gCtx::Instance()->getPid()) {
            LOG_ERROR << " IcmpIdInvalid " << " expect=" << _gCtx::Instance()->getPid() << " actual=" << pid;

            return -1;
        }
        LOG_DEBUG << " CheckPidOk" << " expect=" << _gCtx::Instance()->getPid() << " actual=" << pid;

        LOG_DEBUG << " CheckSeqOk" << " actual=" << seq;

        using muduo::Timestamp;
        Timestamp _send(_body._sendTime);

        _rrt = timeDifference(Timestamp::now(), _send) * 1000 * 1000;
        _isSucess = true;
        LOG_INFO << "icmpPkt CheckOk taskId=" << _body._taskId << " ip=" << IcmpSockets::uint32ToIp(_body._ipInt) <<
                 " rrt=" << timeDifference(Timestamp::now(), _send) <<
                 " s";
        return 0;
    }


    void IcmpPkt::buildHeader(struct iphdr *ip, struct icmphdr *icmp) {
        ip->version = 4;
        ip->ihl = 5;
        ip->tos = 0;
        ip->id = rand();
        ip->frag_off = 0;
        ip->ttl = 255;
        ip->protocol = IPPROTO_ICMP;
        icmp->code = 0;
        icmp->un.echo.sequence = muduo::net::sockets::hostToNetwork16(seq);
        icmp->un.echo.id = muduo::net::sockets::hostToNetwork16(_gCtx::Instance()->getPid());
        icmp->checksum = 0;
    }


    uint16_t IcmpPkt::icmpChecksum(const char *data, int length) {

        uint16_t *data16 = (uint16_t *) data;
        uint32_t sum = 0;
        for (sum = 0; length > 1; length -= 2)
            sum += *data16++;
        if (length == 1)
            sum += *(unsigned char *) data16;

        sum = (sum >> 16) + (sum & 0xffff);
        sum += (sum >> 16);
        return ~sum;

    }


}
