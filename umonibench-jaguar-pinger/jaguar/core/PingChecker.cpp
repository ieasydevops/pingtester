//
// Created by blackwood on 12/11/16.
//

#include <jaguar/core/PingChecker.h>
#include <jaguar/common/PingQue.h>
#include <muduo/base/Logging.h>
#include <jaguar/common/PingTaskLifeCycleManager.h>

using namespace common;

namespace core {


    PingChecker::~PingChecker() {

    }

    void PingChecker::doChecker() {
        while (isRunning) {
            IcmpPktPrt icmpPktPrt = SingletonIcmpPktQ::Instance()->recvIcmpPktQ.take();
            if (icmpPktPrt->buildAndCheck() == 0) {

                PingTaskContextManager::getInstance()->onReceivedIcmpPkt(icmpPktPrt);

                continue;
            }
            LOG_INFO << "CheckOneIcmpPktError IcmpPkt  IsNotSendFromMe";
        }
    }


}
