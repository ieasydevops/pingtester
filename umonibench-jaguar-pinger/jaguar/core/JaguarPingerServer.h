//
// Created by blackwood on 12/8/16.
//

#ifndef UMONIBENCH_JAGUAR_PING_JAGUARPINGERSERVER_H
#define UMONIBENCH_JAGUAR_PING_JAGUARPINGERSERVER_H


#include <muduo/base/Logging.h>
#include <muduo/base/Mutex.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>

#include <boost/bind.hpp>

#include <set>
#include <stdio.h>
#include <jaguar/common/LengthHeaderCodec.h>
#include <jaguar/common/Msg.h>
#include <jaguar/common/PingerMsgCodec.h>
#include <jaguar/common/PingTaskLifeCycleManager.h>


using namespace muduo;
using namespace muduo::net;
using namespace common;


namespace core {


    class JaguarPingerServer : boost::noncopyable {
    public:
        JaguarPingerServer(EventLoop *loop, const InetAddress &listenAddr);

        void start();

        void onTouchResComplete(PingTouchResPtr &pingTouchResPtr);


        void onPingResponseComplete(PingResponsePtr &pingResponsePtr);


    private:


        void onTimer();


        void onConnection(const TcpConnectionPtr &conn);


        void onPingTouchReq(const muduo::net::TcpConnectionPtr &conn, const PingTouchReq &pingTouchReq,
                            muduo::Timestamp timestamp1);


        void onWriteComplete(const TcpConnectionPtr &connectionPtr);


    private:
        typedef std::set<TcpConnectionPtr> ConnectionList;
        TcpServer server_;
        PingerMsgCodec codec_;
        ConnectionList connections_;
    };


    typedef boost::shared_ptr<JaguarPingerServer> JaguarPingerServerPtr;

}


#endif //UMONIBENCH_JAGUAR_PING_JAGUARPINGERSERVER_H
