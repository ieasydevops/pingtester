//
// Created by blackwood on 12/8/16.
//

#ifndef UMONIBENCH_JAGUAR_PING_SIGLETON_H
#define UMONIBENCH_JAGUAR_PING_SIGLETON_H


#include <stddef.h>

namespace common {

    class DefaultLocker {
    public:
        inline void lock() { }

        inline void unlock() { }
    };

    template<class T, class Locker = DefaultLocker>
    class Singleton {
    public:
        Singleton() { }

        virtual ~Singleton() {
            destroyInstance();
        }

        static T *Instance();

        static void destroyInstance();

    private:
        static T *pInstance;
        static Locker m_locker;
    };

    template<class T, class Locker>
    inline T *Singleton<T, Locker>::Instance() {
        if (pInstance == NULL) {
            m_locker.lock();
            if (pInstance == NULL)
                pInstance = new T;
            m_locker.unlock();
        }
        return pInstance;
    }

    template<class T, class Locker>
    void Singleton<T, Locker>::destroyInstance() {
        if (pInstance)
            delete pInstance;
        pInstance = NULL;
    }

    template<class T, class Locker>
    Locker Singleton<T, Locker>::m_locker;

    template<class T, class Locker>
    T *Singleton<T, Locker>::pInstance = NULL;

}

#endif //UMONIBENCH_JAGUAR_PING_SIGLETON_H
