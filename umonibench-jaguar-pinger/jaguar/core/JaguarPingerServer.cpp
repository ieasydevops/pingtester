//
// Created by blackwood on 12/8/16.
//

#include <jaguar/core/JaguarPingerServer.h>

#include <jaguar/common/PingQue.h>
#include <jaguar/common/Singleton.h>

namespace core {

    JaguarPingerServer::JaguarPingerServer(EventLoop *loop, const InetAddress &listenAddr) :
            server_(loop, listenAddr, "JaguarPingerServer"),
            codec_(boost::bind(&JaguarPingerServer::onPingTouchReq, this, _1, _2, _3)) {
        server_.setConnectionCallback(
                boost::bind(&JaguarPingerServer::onConnection, this, _1));
        server_.setMessageCallback(
                boost::bind(&PingerMsgCodec::onMessage, &codec_, _1, _2, _3));
        loop->runEvery(1, boost::bind(&JaguarPingerServer::onTimer, this));

    }


    void JaguarPingerServer::start() {
        server_.start();
    }


    void JaguarPingerServer::onConnection(const TcpConnectionPtr &conn) {
        LOG_INFO << conn->localAddress().toIpPort() << " -> "
                 << conn->peerAddress().toIpPort() << " is "
                 << (conn->connected() ? "UP" : "DOWN");

        if (conn->connected()) {
            connections_.insert(conn);
        }
        else {
            connections_.erase(conn);
        }
    }


    void JaguarPingerServer::onTouchResComplete(PingTouchResPtr &pingTouchResPtr) {
        for (ConnectionList::iterator it = connections_.begin(); it != connections_.end(); ++it) {
            codec_.sendPingTouchRes(*it, pingTouchResPtr);
        }
    }


    void JaguarPingerServer::onPingResponseComplete(PingResponsePtr &pingResponsePtr) {

        for (ConnectionList::iterator it = connections_.begin(); it != connections_.end(); ++it) {



            codec_.sendPingResponse(*it, pingResponsePtr);


        }

    }


    void JaguarPingerServer::onPingTouchReq(const muduo::net::TcpConnectionPtr &conn,
                                            const PingTouchReq &pingTouchReq, muduo::Timestamp timestamp) {

        PingEntryPtr pingEntryPtr;
        pingTouchReq.buildForPingEntryPtr(pingEntryPtr);
        SingletonPingTaskQueue::Instance()->pingTaskQue.put(pingEntryPtr);

        LOG_INFO << "TaskTrace PingTaskReceived [" << " taskId=" << pingEntryPtr->_taskId << " ipCnt=" <<
                 (pingEntryPtr->_ipCnt * pingEntryPtr->_maxSendCnt) << "]" << " receivedTime=" <<
                 muduo::Timestamp::now().microSecondsSinceEpoch();
    }


    void JaguarPingerServer::onTimer() {

    }
}