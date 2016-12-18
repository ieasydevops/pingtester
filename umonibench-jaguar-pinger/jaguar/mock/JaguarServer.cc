//
// Created by blackwood on 12/12/16.
//

#include <muduo/base/Logging.h>
#include <muduo/base/Mutex.h>
#include <muduo/net/EventLoopThread.h>
#include <muduo/net/TcpClient.h>
#include <muduo/net/EventLoop.h>
#include <jaguar/core/JaguarPingerServer.h>
#include <boost/bind.hpp>
#include <boost/function.hpp>

using namespace muduo;
using namespace muduo::net;
using namespace core;


void onMsgArrival(std::string msg) {
    LOG_INFO << "Gloable received one msg= " << msg;
};

class AServer {

public:

    void onMsgArrival(std::string msg) {
        LOG_INFO << "AServer received one msg= " << msg;
    };
};


class BClient {


public:

    typedef boost::function<void(std::string)> MsgCompleteCallBack;

    explicit BClient(const MsgCompleteCallBack &cb) : _myCallBack(cb) {

    }

    void onTimer() {
        _myCallBack("I'am a BClient haha");
    };

private:

    MsgCompleteCallBack _myCallBack;

};


int main() {

    Logger::setLogLevel(Logger::DEBUG);
    LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();
    muduo::net::EventLoop loop;

    InetAddress serverAddr(19999);
    JaguarPingerServer jaguarPingerServer(&loop, serverAddr);
    jaguarPingerServer.start();


    AServer aServer;

//    BClient bClient(onMsgArrival);
//    loop.runEvery(1, boost::bind(&BClient::onTimer, &bClient));
//    loop.loop();
};