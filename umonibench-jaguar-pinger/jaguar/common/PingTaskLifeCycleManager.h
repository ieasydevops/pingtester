//
// Created by blackwood on 12/17/16.
//

#ifndef UMONIBENCH_JAGUAR_PINGER_PINGTASKLIFECYCLEMANAGER_H
#define UMONIBENCH_JAGUAR_PINGER_PINGTASKLIFECYCLEMANAGER_H

#include <jaguar/common/PingEntry.h>
#include <muduo/base/Logging.h>
#include <jaguar/common/IcmpPkt.h>
#include <jaguar/common/Msg.h>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <muduo/base/Atomic.h>
#include <muduo/net/EventLoop.h>


using namespace muduo;

namespace common {


    typedef boost::function<void(PingEntryPtr)> NewPingTouchTaskCallBack;
    typedef boost::function<void(IcmpPktPrt)> IcmpPktReceivedCallBack;

    typedef boost::function<void(PingTouchResPtr)> PingTouchFinishedCallBack;

    typedef boost::function<void(PingTouchResPtr)> TimeoutCallBack;


    struct Res {


        Res(int32_t ip, int32_t Cnt, int32_t cost) {
            _ip = ip;
            _recvCnt = Cnt;
            _cost = cost;
        };

        int32_t _ip;
        int32_t _recvCnt;
        int32_t _cost;
    };


    typedef std::map<int32_t, Res> ResMapper;
    typedef ResMapper::iterator resIt;


    typedef boost::shared_ptr<std::queue<IcmpPktPrt> > IcmpPktPtrQPtr;

    struct PingResponse {
        PingResponse(int64_t taskId, int32_t totalCnt) {
            _taskId = taskId;
            _totalCnt = totalCnt;
            _sucessCnt.getAndSet(0);
            _isSucess = false;
            _benginTime = muduo::Timestamp::now();
            icmpPktPtrQPtr = IcmpPktPtrQPtr(new std::queue<IcmpPktPrt>());
        };


        void mergeRes(IcmpPktPrt icmpPktPrt) {

            assert(_taskId == icmpPktPrt->_body._taskId);

            icmpPktPtrQPtr->push(icmpPktPrt);


            resIt it = _result.find(icmpPktPrt->_body._ipInt);
            if (it != _result.end()) {

                (*it).second._recvCnt += 1;
                (*it).second._cost += icmpPktPrt->_rrt;

            } else {

                Res res(icmpPktPrt->_body._ipInt, 1, icmpPktPrt->_rrt);
                _result.insert(std::make_pair(icmpPktPrt->_body._ipInt, res));

            }

            _isSucess = (_sucessCnt.incrementAndGet() >= _totalCnt);
            LOG_DEBUG << "TaskTrace doMerge " << " taskId=" << _taskId << " sucessCnt=" << _sucessCnt.get() <<
                      " isSucess=" << _isSucess;
        }


        void writeTo(muduo::net::Buffer *pBuffer) {

            int32_t len = NormalHeaderLen + 4 + ONE_RESP_PING_SIZE * _sucessCnt.get();

            LOG_DEBUG << "TaskTrace receivCnt=" << _sucessCnt.get() << " icmpQ=" << icmpPktPtrQPtr->size() << " len=" <<
                      len;

            PingerHeader _pHeader(len, PingRes, PKG_VERION, _taskId, PKG_TIMEOUT, _benginTime.microSecondsSinceEpoch());

            pBuffer->ensureWritableBytes(len);
            _pHeader.writeTo(pBuffer);
            pBuffer->appendInt32(_sucessCnt.get());

            while (icmpPktPtrQPtr->size()) {
                IcmpPktPrt icmpPkgPtr = icmpPktPtrQPtr->front();
                icmpPkgPtr->writeToSockBuf(pBuffer);
                icmpPktPtrQPtr->pop();
            }

        }


        bool IsSucess() {
            return _isSucess;
        }

        bool _isSucess;
        int64_t _taskId;
        int32_t _totalCnt;
        AtomicInt32 _sucessCnt;
        Timestamp _benginTime;
        std::map<int32_t, Res> _result;
        IcmpPktPtrQPtr icmpPktPtrQPtr;

    };


    typedef boost::shared_ptr<PingResponse> PingResponsePtr;

    typedef boost::function<void(PingResponsePtr)> PingResponseCallBack;


    class PingTaskContext {
    public:
        PingTaskContext(PingEntryPtr &req) {
            _req = req;
            _res = PingResponsePtr(new PingResponse(req->_taskId, (req->_ipCnt * req->_maxSendCnt)));
        };

        void IcmpPktPrtReceived(IcmpPktPrt &icmpPktPrt) {
            _res->mergeRes(icmpPktPrt);
        };

        bool IsSucess() {
            return _res->IsSucess();
        }


        bool IsTimeOut() {
            return (timeDifference(Timestamp::now(), _res->_benginTime) > (static_cast<double>(_req->_timeOut)/2000));
        }


        PingResponsePtr &getResponse() {
            return _res;
        }

    private:
        PingEntryPtr _req;
        PingResponsePtr _res;
    };


    typedef boost::shared_ptr<PingTaskContext> PingTaskContextPtr;


    class PingTaskContextManager {


    public:

        typedef std::map<int64_t, PingTaskContextPtr> PingTaskContextMapper;
        typedef PingTaskContextMapper::iterator mapperIterator;
        typedef std::map<int64_t, PingTaskContextPtr>::size_type t_size;


    public:
        void onTaskFinishedOrTimeOut(PingResponsePtr pingResponsePtr) {
            muduo::MutexLockGuard lockGuard(_mapperLock);
            LOG_DEBUG << "TaskTrace TaskFinished taskId=" << pingResponsePtr->_taskId << " Finished" << " costTime=" <<
                      (timeDifference(Timestamp::now(), pingResponsePtr->_benginTime)) << "s";
            this->pingTaskContextMapper.erase(pingResponsePtr->_taskId);
            _CallBack(pingResponsePtr);
        }


    public:

        static PingTaskContextManager *getInstance() {
            static PingTaskContextManager *instance = new PingTaskContextManager();
            return instance;
        };


    public:


        void registPingResponseCallBack(const PingResponseCallBack &finishedCallBack) {
            _CallBack = finishedCallBack;
        };


        void onNewPingTaskExecution(PingEntryPtr pingEntryPtr) {

            muduo::MutexLockGuard lockGuard(_mapperLock);

            PingTaskContextPtr pingTaskContextPtr = PingTaskContextPtr(new PingTaskContext(pingEntryPtr));
            this->pingTaskContextMapper.insert(std::make_pair(pingEntryPtr->_taskId, pingTaskContextPtr));

            LOG_INFO << "TaskTrace onNewPingTaskExecution " << " taskId=" << pingEntryPtr->_taskId << " totalCnt=" <<
                     (pingEntryPtr->_ipCnt * pingEntryPtr->_maxSendCnt);
        }


        void onReceivedIcmpPkt(IcmpPktPrt icmpPktPrt) {
            mapperIterator iterator = this->pingTaskContextMapper.find(icmpPktPrt->_body._taskId);
            if (iterator != pingTaskContextMapper.end()) {
                (*iterator).second->IcmpPktPrtReceived(icmpPktPrt);
                if ((*iterator).second->IsSucess() || ((*iterator).second->IsTimeOut())) {
                    LOG_INFO << "TaskTrace taskFinished " << " taskId=" << (*iterator).second->getResponse()->_taskId;
                    this->onTaskFinishedOrTimeOut((*iterator).second->getResponse());
                }
            }
        };


    private:


        PingTaskContextManager() {
            LOG_DEBUG << "PingTaskContextManager build once";
        };


    private:
        muduo::MutexLock _mapperLock;
        PingTaskContextMapper pingTaskContextMapper;
        PingResponseCallBack _CallBack;
    };


}


#endif //UMONIBENCH_JAGUAR_PINGER_PINGTASKLIFECYCLEMANAGER_H
