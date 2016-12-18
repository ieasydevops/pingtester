//
// Created by blackwood on 12/13/16.
//

#include <jaguar/core/PingReporter.h>

#include <jaguar/common/IcmpPkt.h>
#include <queue>
#include <jaguar/common/Msg.h>
#include <jaguar/common/PingQue.h>

using namespace common;


namespace core {

//
//    void PingReporter::onTouchResComplete(PingTouchResPtr &pingTouchResPtr) {
//        _server.onTouchResComplete(pingTouchResPtr);
//    }

    void PingReporter::onPingResponseCompleted(PingResponsePtr &pingResPtr) {


        LOG_DEBUG << "TaskTrace PingResponseCompleted " << " taskId=" << pingResPtr->_taskId << " costTime=" <<
                  timeDifference(Timestamp::now(), pingResPtr->_benginTime) << " sucessCnt=" <<
                  (pingResPtr->_sucessCnt.get());

        _server.onPingResponseComplete(pingResPtr);

//        LOG_DEBUG << "TaskTrace detail ";
//        for (ResMapper::iterator it = pingResPtr->_result.begin(); it != pingResPtr->_result.end(); ++it) {
//
//            LOG_DEBUG << "TaskTrace ip=" << it->first << " cnt=" << it->second._recvCnt << " cost=" << it->second._cost;
//
//
//        }


    }


}