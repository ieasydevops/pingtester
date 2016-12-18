//
// Created by blackwood on 12/3/16.
//

#include <jaguar/core/PingTaskGen.h>
#include <stdio.h>
#include <string.h>
#include <muduo/base/Logging.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <jaguar/common/PingEntry.h>

#include <jaguar/common/PingQue.h>

#include <jaguar/common/PingTaskLifeCycleManager.h>

using namespace common;


namespace core {


    PingTaskGen::~PingTaskGen() {


    }


    void PingTaskGen::doTaskGen() {
        while (isRunning) {
            loadPingTaskFromFile(pfileName);
            usleep(10 * 1000);
        }
    }


    void PingTaskGen::loadPingTaskFromFile(const char *fileName) {

        FILE *ping_file;
        char line[256];
        char host[256];
        if (strcmp(fileName, "-") == 0)
            ping_file = fdopen(0, "r");
        else
            ping_file = fopen(fileName, "r");
        if (!ping_file) {
            LOG_ERROR << "ErrorCode=" << "FileNoExsist" << " ErrorMsg=" << "LoadPingTaskFromFileFailed";
            return;
        }

        std::vector<uint32_t> addrs;
        while (fgets(line, sizeof(line), ping_file)) {
            if (sscanf(line, "%s", host) != 1)
                continue;
            if ((!*host) || (host[0] == '#'))  /* magic to avoid comments */
                continue;

            std::string ipStr = std::string(line).substr(0, std::string(line).find_last_not_of(" \n\r\t") + 1);
            in_addr_t addr = inet_addr(ipStr.c_str());
            addrs.push_back(addr);
        }
        int32_t *ips = new int32_t[addrs.size()];
        for (int i = 0; i < (addrs.size()); i++) {
            ips[i] = addrs[i];
        }

        PingEntryPtr pingEntryPtr = PingEntryPtr(new PingEntry(ips, addrs.size(), 1, _taskId.incrementAndGet(), 50,
                                                               Timestamp::now().kMicroSecondsPerSecond, 1));
        LOG_DEBUG << "GenOnePingEntry [" << pingEntryPtr->toString() << "]";

//        PingTaskLifeCycleManager::getInstance()->onNewTask(pingEntryPtr);

        PingTaskContextManager::getInstance()->onNewPingTaskExecution(pingEntryPtr);

        fclose(ping_file);
    }


}