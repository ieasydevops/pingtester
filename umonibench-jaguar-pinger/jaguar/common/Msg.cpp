//
// Created by blackwood on 12/12/16.
//

#include "Msg.h"


namespace common {


    PingerMsg::PingerMsg() {

    }


    PingerMsg::~PingerMsg() {


    }


    PingTouchReq::PingTouchReq() {

    }


    PingTouchReq::~PingTouchReq() {

        if (ips != NULL) {
            delete[](ips);
            ips = NULL;
        }

    }


    void PingTouchReq::writeTo(muduo::net::Buffer *pBuffer) {

    }


    void  PingTouchReq::printSelf() const {
        LOG_INFO << "PingTouchTaskId=" << _pHeader._reqId << " len=" << _pHeader._len << " time=" <<
                 _pHeader._startTime <<
                 " seq=" << _seq << " touchCount=" << _touchPkgSize << " ipCnt="
                 << _ipCnt << " ip[0]=" << (_ipCnt > 0 ? ips[0] : 0);
    }


    void PingTouchReq::readFrom(muduo::net::Buffer *pBuffer) {
        _pHeader.readFrom(pBuffer);
        _seq = pBuffer->readInt16();
        _touchCount = pBuffer->readInt16();
        _touchPkgSize = pBuffer->readInt16();
        _ipCnt = pBuffer->readInt32();
        ips = new int32_t[_ipCnt];
        for (int i = 0; i < _ipCnt; i++) {
            ips[i] = pBuffer->readInt32();
        }
    }


    void PingTouchReq::buildForPingEntryPtr(PingEntryPtr &pingEntryPtr) const {
        int32_t *addrs = new int32_t[_ipCnt];
        for (int i = 0; i < _ipCnt; i++) {
            addrs[i] = ips[i];
        }
        pingEntryPtr = PingEntryPtr(
                new PingEntry(addrs, _ipCnt, _touchCount, _pHeader._reqId, _touchPkgSize, _pHeader._startTime,
                              _pHeader._timeout));
    }


    PingTouchRes::PingTouchRes(int64_t taskId, int32_t ipCnt) : icmpPktPtrQ(new std::queue<IcmpPktPrt>) {
        _taskId = taskId;
        _totalCnt = ipCnt;
        _receivedReqTime = Timestamp::now().microSecondsSinceEpoch();
    }


    PingTouchRes::~PingTouchRes() {

        LOG_DEBUG << "PingTouchRes destory";
    }


    PingTouchRes::PingTouchRes(std::queue<IcmpPktPrt> &queue) : icmpPktPtrQ(new std::queue<IcmpPktPrt>) {
        while (queue.size()) {
            this->icmpPktPtrQ->push(queue.front());
            queue.pop();
        }
    }

    void PingTouchRes::onIcmpPktPrt(IcmpPktPrt icmpPktPrt) {
        this->icmpPktPtrQ->push(icmpPktPrt);
    }


    void PingTouchRes::writeTo(muduo::net::Buffer *pBuffer) {
        _pHeader._len = NormalHeaderLen + 4 + ONE_RESP_PING_SIZE * icmpPktPtrQ->size();
        _pHeader._type = PingRes;
        _pHeader._reqId = _taskId;
        _pHeader._version = PKG_VERION;
        _pHeader._timeout = PKG_TIMEOUT;
        _recvCnt = icmpPktPtrQ->size();
        pBuffer->ensureWritableBytes(_pHeader._len);
        _pHeader.writeTo(pBuffer);
        pBuffer->appendInt32(_recvCnt);


        while (icmpPktPtrQ->size()) {
            IcmpPktPrt icmpPkgPtr = icmpPktPtrQ->front();
            icmpPkgPtr->writeToSockBuf(pBuffer);
            icmpPktPtrQ->pop();
        }
    }


    void PingTouchRes::readFrom(muduo::net::Buffer *buffer) {

    }


    int64_t PingTouchRes::getTaskId() {
        return _taskId;
    }

}