//
// Created by blackwood on 12/12/16.
//

#ifndef UMONIBENCH_JAGUAR_PINGER_MSG_H
#define UMONIBENCH_JAGUAR_PINGER_MSG_H

#include <stdint.h>
#include <muduo/net/Buffer.h>
#include <boost/shared_ptr.hpp>
#include <muduo/base/Logging.h>
#include <queue>
#include <boost/smart_ptr/scoped_ptr.hpp>
#include "IcmpPkt.h"
#include "PingEntry.h"

namespace common {


    static const int32_t NormalHeaderLen = 4 //start
                                           + 4 //len
                                           + 1 //type
                                           + 1 //version
                                           + 8 //reqId
                                           + 8 //timeout
                                           + 8 //startTime
    ;

    static const int32_t PROBER_PREFIX = 55860;


    static const int ONE_RESP_PING_SIZE = 17;


    static const int PKG_VERION = 0;
    static const long PKG_TIMEOUT = 1000;


    enum MsgType {
        PingReq = 2,
        PingAck = 52,
        PingRes = 53,
        UnKnown = 0,

    };

    static MsgType byte2MsgType(int8_t _type) {
        MsgType type = UnKnown;
        switch (_type) {
            case 2:
                type = PingReq;
                break;
            case 52:
                type = PingAck;
                break;
            case 53:
                type = PingRes;
                break;
            default:
                type = UnKnown;
                break;
        }
        return type;
    };


    struct PingerHeader {

        int32_t _prefix;
        int32_t _len;
        int8_t _version;
        int8_t _type;
        int64_t _reqId;
        int64_t _timeout;
        int64_t _startTime;


        PingerHeader() {
            this->_version = 0;
            this->_reqId = 0;
            this->_type = 0;
        };

        PingerHeader(int8_t version, int64_t seq, int8_t type, int32_t bodyLen) {
            this->_version = version;
            this->_reqId = seq;
            this->_type = type;
            this->_len = NormalHeaderLen + bodyLen;
        };


        PingerHeader(int32_t len, int8_t type, int8_t version, int64_t taskId, int64_t timeout, int64_t startTime) {
            this->_len = len;
            this->_type = type;
            this->_version = version;
            this->_reqId = taskId;
            this->_timeout = timeout;
            this->_startTime = startTime;
        };

        void writeTo(muduo::net::Buffer *buffer) {
            buffer->ensureWritableBytes(NormalHeaderLen);
            buffer->appendInt32(PROBER_PREFIX);
            buffer->appendInt32(this->_len);
            buffer->appendInt8(this->_type);
            buffer->appendInt8(this->_version);
            buffer->appendInt64(this->_reqId);
            buffer->appendInt64(this->_timeout);
            buffer->appendInt64(this->_startTime);
        }


        void readFrom(muduo::net::Buffer *pBuffer) {
            _prefix = pBuffer->readInt32();
            _len = pBuffer->readInt32();
            _type = pBuffer->readInt8();
            _version = pBuffer->readInt8();
            _reqId = pBuffer->readInt64();
            _timeout = pBuffer->readInt64();
            _startTime = pBuffer->readInt64();
            LOG_DEBUG << "readHeaderDone " << " prefix=" << _prefix << " len=" << _len << " reqId=" << _reqId <<
                      " version=" << _version;
        }

    };


    typedef boost::shared_ptr<PingerHeader> PingerHeaderPtr;


    class PingerMsg {

    protected:
        PingerHeader _pHeader;
    public:

        PingerMsg();

        virtual ~PingerMsg();

        virtual void writeTo(muduo::net::Buffer *pBuffer) = 0;

        virtual void readFrom(muduo::net::Buffer *pBuffer) = 0;

        MsgType getMsgType() {
            return byte2MsgType(_pHeader._type);
        }
    };


    typedef boost::shared_ptr<PingerMsg> PingerMsgPtr;


    class PingTouchReq : public PingerMsg {

    public:

        PingTouchReq();

        virtual ~PingTouchReq();

        void readFrom(muduo::net::Buffer *pBuffer);

        void writeTo(muduo::net::Buffer *pBuffer);

        void buildForPingEntryPtr(PingEntryPtr &pingEntryPtr) const;

        void printSelf() const;


    private:
        int8_t _seq;
        int8_t _touchCount;
        int8_t _touchPkgSize;
        int32_t _ipCnt;
        int32_t *ips;

    };


    typedef boost::shared_ptr<PingTouchReq> PingTouchReqPtr;

    typedef boost::scoped_ptr<std::queue<IcmpPktPrt> > IcmpPktPtrQ;


    class PingTouchRes : public PingerMsg {
    public:
        PingTouchRes(int64_t taskId, int32_t ipCnt);

        ~PingTouchRes();

        PingTouchRes(std::queue<IcmpPktPrt> &queue);

        void onIcmpPktPrt(IcmpPktPrt icmpPktPrt);

        void writeTo(muduo::net::Buffer *pBuffer);

        void readFrom(muduo::net::Buffer *buffer);

        int64_t getTaskId();

        int64_t _receivedReqTime;
        int32_t _recvCnt;
        int32_t _totalCnt;
        int64_t _taskId;

    private:
        boost::scoped_ptr<std::queue<IcmpPktPrt> > icmpPktPtrQ;

    };

    typedef boost::shared_ptr<PingTouchRes> PingTouchResPtr;


}


#endif //UMONIBENCH_JAGUAR_PINGER_MSG_H
