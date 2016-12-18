//
// Created by blackwood on 12/2/16.
//


#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/base/Thread.h>
#include <muduo/base/ThreadPool.h>
#include <jaguar/core/JaguarPingerServer.h>
#include <jaguar/core/PingReceiver.h>
#include <jaguar/core/PingExecutor.h>
#include <jaguar/core/PingChecker.h>
#include <jaguar/core/PingReporter.h>
#include <jaguar/core/PingCore.h>
#include <jaguar/common/PingTaskLifeCycleManager.h>

namespace core {


    int PingCore::start(int argc, char **argv) {

        boost::scoped_ptr<muduo::LogFile> g_logFile;
        g_logFile.reset(new muduo::LogFile(::basename("jaguar"), kRollSize, true, 1));
        Logger::setLogLevel(Logger::DEBUG);

        LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();
        muduo::net::EventLoop loop;
        InetAddress serverAddr(19999);
        JaguarPingerServer jaguarPingerServer(&loop, serverAddr);


        PingExecutor pingExecutor;
        muduo::Thread sender(boost::bind(&PingExecutor::doExecution, boost::ref(pingExecutor)));


        PingReceiver pingReceiver;
        muduo::Thread receiver(boost::bind(&PingReceiver::doReceiver, boost::ref(pingReceiver)));


        PingChecker pingChecker;
        muduo::Thread checker(boost::bind(&PingChecker::doChecker, boost::ref(pingChecker)));


        PingReporter reporter(jaguarPingerServer);

        PingTaskContextManager::getInstance()->registPingResponseCallBack(
                boost::bind(&PingReporter::onPingResponseCompleted, boost::ref(reporter), _1));


        jaguarPingerServer.start();
        sender.start();
        receiver.start();
        checker.start();

        loop.loop();
        sender.join();
        receiver.join();
        checker.join();
        return 0;
    }
}


