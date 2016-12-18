//
// Created by blackwood on 12/9/16.
//
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <queue>
#include <jaguar/common/IcmpSockets.h>

#include <jaguar/core/PingExecutor.h>
#include <jaguar/common/PingQue.h>
#include <muduo/base/Logging.h>
#include <errno.h>
#include <arpa/inet.h>
#include <jaguar/common/IcmpPkt.h>
#include <jaguar/common/Context.h>
#include <jaguar/common/PingTaskLifeCycleManager.h>

using namespace common;

namespace core {

    PingExecutor::~PingExecutor() {


    }


    void PingExecutor::setUp() {
        sockfd = common::IcmpSockets::GetSenderIcmpSocket();
        assert(sockfd > 0);

    }

    void PingExecutor::doExecution() {
        while (isRunning) {
            PingEntryPtr pingEntryPtr = SingletonPingTaskQueue::Instance()->pingTaskQue.take();
            pingTask(pingEntryPtr);
        }
    }


    void PingExecutor::pingTask(PingEntryPtr pingEntryPtr) {

//        PingTaskLifeCycleManager::getInstance()->onNewTask(pingEntryPtr);

        PingTaskContextManager::getInstance()->onNewPingTaskExecution(pingEntryPtr);


        int sucessCount = 0;
        for (int ipIndex = 0; ipIndex < pingEntryPtr->_ipCnt; ipIndex++) {
            int tryCount = 0;
            uint32_t targetIp = pingEntryPtr->_ipTargets[ipIndex];
            for (int sendCnt = 0; sendCnt < pingEntryPtr->_maxSendCnt;) {

                IcmpPktPrt icmpPktPrt(new IcmpPkt(targetIp, pingEntryPtr->_pktSize,
                                                  pingEntryPtr->_taskId,
                                                  ipIndex));

                int sendRet = icmpPktPrt->writeTo(sockfd);


                if (sendRet >= 0) {

                    sendCnt++;

                    sucessCount++;


                    LOG_DEBUG << "SendIcmpPktOk " << " ip=" << targetIp << " taskId=" << pingEntryPtr->_taskId <<
                              " sendTime=" << muduo::Timestamp::now().microSecondsSinceEpoch();


                } else {


                    LOG_ERROR << "ErrorCode=" << "WriteToSockFd(" << sockfd << ")" << "Failed" << "  ErrMsg=" <<
                    strerror(errno);


                    if (errno == 105) {

                        if (tryCount++ > 50) {
//                                fip.fCt = pingEntryPtr->num_max_sendCount - sendCnt;

                            tryCount = 0;
//                                ack.failIps.push_back(fip);
                            sendCnt++;
                            LOG_DEBUG << "SendIcmpPktFailed " << " ip=" << pingEntryPtr->_ipTargets[ipIndex] <<
                                      " taskId=" <<
                                      pingEntryPtr->_taskId << " tryNu=" << tryCount << " errno=" << errno <<
                                      " errMsg=" << strerror(errno);


                        }
                    } else {

//                            fip.fCt = pingEntryPtr->num_max_sendCount - sendCnt;

                        LOG_DEBUG << "SendIcmpPktFailed " << " ip=" << pingEntryPtr->_ipTargets[ipIndex] <<
                                  " taskId=" <<
                                  pingEntryPtr->_taskId << " errno=" << errno << " errMsg=" << strerror(errno);

//                            ack.failIps.push_back(fip);
                        break;
                    }

                }
            }

        }


    }

}