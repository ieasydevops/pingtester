//
// Created by blackwood on 12/14/16.
//



#include <jaguar/common/PingTaskLifeCycleManager.h>
#include <jaguar/common/PingQue.h>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <jaguar/core/PingTaskGen.h>
#include <muduo/base/Thread.h>
#include <jaguar/core/PingExecutor.h>
#include <jaguar/core/PingReceiver.h>
#include <jaguar/core/PingChecker.h>
#include <muduo/base/LogFile.h>
#include <jaguar/core/PingCore.h>


using namespace muduo;
using namespace common;
using namespace core;


int main() {


    boost::scoped_ptr<muduo::LogFile> g_logFile;
    g_logFile.reset(new muduo::LogFile(::basename("jaguar"), kRollSize, true, 1));
    Logger::setLogLevel(Logger::INFO);



    PingTaskGen pingTaskGen("./etc/pingTargets.txt");
    muduo::Thread taskGen(boost::bind(&PingTaskGen::doTaskGen, boost::ref(pingTaskGen)));
    taskGen.start();


    PingExecutor pingExecutor;
    muduo::Thread sender(boost::bind(&PingExecutor::doExecution, boost::ref(pingExecutor)));
    sender.start();


    PingReceiver pingReceiver;
    muduo::Thread receiver(boost::bind(&PingReceiver::doReceiver, boost::ref(pingReceiver)));
    receiver.start();


    PingChecker pingChecker;
    muduo::Thread checker(boost::bind(&PingChecker::doChecker, boost::ref(pingChecker)));
    checker.start();


    taskGen.join();
    receiver.join();
    checker.join();
    sender.join();

};








