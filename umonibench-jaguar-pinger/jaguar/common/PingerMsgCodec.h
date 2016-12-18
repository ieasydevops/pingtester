//
// Created by blackwood on 12/12/16.
//

#ifndef UMONIBENCH_JAGUAR_PINGER_PINGERMSGCODEC_H
#define UMONIBENCH_JAGUAR_PINGER_PINGERMSGCODEC_H

#include <muduo/base/Types.h>
#include <muduo/base/Logging.h>
#include <muduo/net/Endian.h>
#include <muduo/net/TcpConnection.h>
#include <jaguar/common/Msg.h>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include "PingTaskLifeCycleManager.h"


namespace common {


    const string kNoErrorStr = "NoError";
    const string kInvalidLengthStr = "InvalidLength";
    const string kInvalidPREFIXStr = "PREFIXInvalid";
    const string kUnknownMessageTypeStr = "UnknownMessageType";
    const string kParseErrorStr = "ParseError";
    const string kUnknownErrorStr = "UnknownError";


    class PingerMsgCodec : boost::noncopyable {


    public:

        enum ErrorCode {
            kNoError = 0,
            kInvalidLength,
            kInvalidPREFIX,
            kUnknownMessageType,
            kParseError,
        };


    public:

        typedef boost::function<void(const muduo::net::TcpConnectionPtr &, const PingTouchReq &,
                                     muduo::Timestamp
        )> PingTouchReqCallback;





        typedef boost::function<void(const muduo::net::TcpConnectionPtr &, muduo::net::Buffer *,
                                     muduo::Timestamp,
                                     ErrorCode)> ErrorCallback;


        explicit PingerMsgCodec(const PingTouchReqCallback &cb)
                : pingTouchReqCallback_(cb), errorCallback_(defaultErrorCallback) {

        }


        PingerMsgCodec(const PingTouchReqCallback &messageCb, const ErrorCallback &errorCb)
                : pingTouchReqCallback_(messageCb),
                  errorCallback_(errorCb) {
        }


        void onMessage(const muduo::net::TcpConnectionPtr &conn,
                       muduo::net::Buffer *buf,
                       muduo::Timestamp receiveTime);








        void sendPingTouchRes(const muduo::net::TcpConnectionPtr &conn,const common::PingTouchResPtr &pingTouchResPtr) const;

        void sendPingResponse(const muduo::net::TcpConnectionPtr &conn,const PingResponsePtr &pingTouchResPtr) const;








        static void defaultErrorCallback(const muduo::net::TcpConnectionPtr &,
                                         muduo::net::Buffer *,
                                         muduo::Timestamp,
                                         ErrorCode);


        static const string &errorCodeToString(ErrorCode errorCode);


        static void fillEmptyBuffer(muduo::net::Buffer *buf, const PingTouchResPtr &pingerMsg);


    private:
        PingTouchReqCallback pingTouchReqCallback_;
        ErrorCallback errorCallback_;

    };


}


#endif //UMONIBENCH_JAGUAR_PINGER_PINGERMSGCODEC_H
