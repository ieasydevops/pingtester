//
// Created by blackwood on 12/12/16.
//

#include <jaguar/common/PingerMsgCodec.h>


namespace common {


    void PingerMsgCodec::onMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buf,
                                   muduo::Timestamp receiveTime) {


        while (buf->readableBytes() >= NormalHeaderLen) // kHeaderLen == 4
        {

            int index = 0;
            int32_t prefix = buf->peekInt32();
            index += 4;
            LOG_DEBUG << "Peek Prefix=" << prefix;
            if (prefix != PROBER_PREFIX) {
                LOG_ERROR << "Invalid Prefix " << prefix;
                errorCallback_(conn, buf, receiveTime, kInvalidPREFIX);
                break;
            }
            int32_t len = IcmpSockets::peekInt32From(index, buf);
            index += 4;

            LOG_DEBUG << "Peek Len=" << len;

            if (len > 65536 || len < 0) {
                LOG_ERROR << "Invalid length " << len;
                errorCallback_(conn, buf, receiveTime, kInvalidLength);
                break;
            } else if (buf->readableBytes() >= len) {
                PingTouchReq pingTouchReq;
                pingTouchReq.readFrom(buf);
                pingTouchReqCallback_(conn, pingTouchReq, receiveTime);
            }
            else {

                break;
            }
        }
    }


    void PingerMsgCodec::sendPingTouchRes(const muduo::net::TcpConnectionPtr &conn,
                                          const common::PingTouchResPtr &pingTouchResPtr) const {
        muduo::net::Buffer buf;
        fillEmptyBuffer(&buf, pingTouchResPtr);
        conn->send(&buf);
        LOG_INFO << "TraceTaskRepotedOk  taskId=" << pingTouchResPtr->getTaskId() << " totalCnt=" <<
                 pingTouchResPtr->_totalCnt << " receivedCnt=" <<
                 pingTouchResPtr->_recvCnt << " costTime=" <<
                 (static_cast<double>(Timestamp::now().microSecondsSinceEpoch() - pingTouchResPtr->_receivedReqTime) /
                  muduo::Timestamp::kMicroSecondsPerSecond) << " s";
    }


    void PingerMsgCodec::sendPingResponse(const muduo::net::TcpConnectionPtr &conn,
                                          const PingResponsePtr &pingResponsePtr) const {

        muduo::net::Buffer buf;
        pingResponsePtr->writeTo(&buf);
        conn->send(&buf);
        LOG_INFO << "TaskTrace RepotedOk  taskId=" << pingResponsePtr->_taskId << " totalCnt=" <<
                 pingResponsePtr->_totalCnt << " receivedCnt=" <<
                 pingResponsePtr->_sucessCnt.get() << " costTime=" <<
                 timeDifference(Timestamp::now(), pingResponsePtr->_benginTime) << " s";

    }


    void PingerMsgCodec::fillEmptyBuffer(muduo::net::Buffer *buf, const common::PingTouchResPtr &pingTouchResPtr) {
        pingTouchResPtr->writeTo(buf);
    }

    const string &PingerMsgCodec::errorCodeToString(ErrorCode errorCode) {
        switch (errorCode) {
            case kNoError:
                return kNoErrorStr;
            case kInvalidLength:
                return kInvalidLengthStr;
            case kInvalidPREFIX:
                return kInvalidPREFIXStr;
            case kUnknownMessageType:
                return kUnknownMessageTypeStr;
            case kParseError:
                return kParseErrorStr;
            default:
                return kUnknownErrorStr;
        }
    }


    void PingerMsgCodec::defaultErrorCallback(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buf,
                                              Timestamp, ErrorCode errorCode) {

        LOG_ERROR << "PingerMsgCodec::defaultErrorCallback - " << errorCodeToString(errorCode);
        if (conn && conn->connected()) {
            conn->shutdown();
        }
    }


}