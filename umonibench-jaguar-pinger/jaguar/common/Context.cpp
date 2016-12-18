//
// Created by blackwood on 12/11/16.
//

#include <jaguar/common/Context.h>
#include <unistd.h>



namespace common {

    Context::Context() {
        _pid = ::getpid();



    }

    Context::~Context() {

    }


    uint32_t Context::getPid() {
        return _pid;
    }



}
