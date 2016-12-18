//
// Created by blackwood on 12/2/16.
//

#ifndef UMONIBENCH_JAGUAR_PING_PINGENTRY_H
#define UMONIBENCH_JAGUAR_PING_PINGENTRY_H

#include <netinet/in.h>

#include <map>
#include  <vector>
#include <boost/shared_ptr.hpp>


namespace common {

    static const int ICMP_DATA_FIXLEN = 2 + 4 + 8 + 8;
    static const int ICMP_MIMI_LEN = 8;
    static const int DEFAULT_PING_DATA_SIZE = 1;

    class PingEntry {
    public:
        PingEntry();

        PingEntry(int32_t *ips, uint32_t ipsCount, uint16_t sendCount, uint64_t taskId,
                  uint16_t pkgSize, int64_t startTime,int64_t timeOut);

        std::string toString();

        ~PingEntry();

        int32_t *_ipTargets;
        uint32_t _ipCnt;
        uint64_t _taskId;
        uint16_t _maxSendCnt;
        uint16_t _pktSize;
        int64_t _startTime;
        int64_t _timeOut;
    };

    typedef boost::shared_ptr<PingEntry> PingEntryPtr;
}


#endif //UMONIBENCH_JAGUAR_PING_PINGENTRY_H
