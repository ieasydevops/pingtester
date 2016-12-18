//
// Created by blackwood on 12/2/16.
//

#include <jaguar/common/IcmpSockets.h>
#include <jaguar/common/PingEntry.h>
#include <stdio.h>
#include <muduo/base/Logging.h>


namespace common {


    PingEntry::PingEntry() {
    }


    PingEntry::PingEntry(int32_t *ips, uint32_t ipsCount, uint16_t sendCount, uint64_t taskId,
                         uint16_t pkgSize, int64_t startTime, int64_t timeOut) {

        _ipTargets = ips;
        _ipCnt = ipsCount;
        _maxSendCnt = sendCount >= 1 ? sendCount : 1;
        _taskId = taskId;
        _pktSize = pkgSize;
        _startTime = startTime;
        _timeOut = timeOut;

    }


    std::string PingEntry::toString() {
        char str[500] = {0};
        snprintf(str, sizeof(str),
                 "taskId=%lu ipCnt=%d pktSize=%u ipTargets[0]=%u ipTargetsNet[0]=%u ipTargets[0]Str=%s", this->_taskId,
                 _ipCnt, _pktSize, this->_ipTargets[0], muduo::net::sockets::hostToNetwork32(this->_ipTargets[0]),
                 common::IcmpSockets::uint32ToIp(this->_ipTargets[0]).c_str());
        return std::string(str);
    }

    PingEntry::~PingEntry() {
        LOG_DEBUG << "PingEntry destory " << this->toString();
        delete[] _ipTargets;
    }

}