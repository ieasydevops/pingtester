//
// Created by blackwood on 12/9/16.
//

#ifndef UMONIBENCH_JAGUAR_PING_PINGQUE_H
#define UMONIBENCH_JAGUAR_PING_PINGQUE_H

#include <muduo/base/BlockingQueue.h>
#include <jaguar/common/PingEntry.h>
#include "Singleton.h"
#include "IcmpPkt.h"
#include <jaguar/common/Msg.h>

namespace common {

    class PingTaskQueue {
    public:
        muduo::BlockingQueue<PingEntryPtr> pingTaskQue;
    };

    typedef Singleton<PingTaskQueue, muduo::MutexLock> SingletonPingTaskQueue;


    class PingReceiverQueue {
    public:
        muduo::BlockingQueue<IcmpPktPrt> recvIcmpPktQ;
    private:
        // no op
    };

    typedef Singleton<PingReceiverQueue, muduo::MutexLock> SingletonIcmpPktQ;


    class IcmpCheckedQueue {
    public:
        muduo::BlockingQueue<IcmpPktPrt> icmpCheckedQ;

    private:
        // no op
    };

    typedef Singleton<IcmpCheckedQueue, muduo::MutexLock> SingletonIcmpCheckedQ;


}


#endif //UMONIBENCH_JAGUAR_PING_PINGQUE_H
