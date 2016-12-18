//
// Created by blackwood on 12/14/16.
//


#include <muduo/base/Logging.h>
#include <boost/bind/bind.hpp>
#include <boost/function.hpp>

class Penguin {
public:

    void run() {
        LOG_INFO << "Penguin Run";
    };

    void swim() {
        LOG_INFO << "Penguin swim";
    };
};


class Sparrow {

public:

    void fly() {
        LOG_INFO << "Sparrow fly";
    };

    void run() {

        LOG_INFO << "Sparrow run";
    };

};


typedef boost::function<void()> SwimCallBack;
typedef boost::function<void()> FlyCallBack;
typedef boost::function<void()> RunCallBack;


class Foo {

public:


    Foo(FlyCallBack flyCallBack, RunCallBack runCallBack) : flyCallBack_(flyCallBack), runCallBack_(runCallBack) {

    }

    void doAction() {
        flyCallBack_();
        runCallBack_();
    };

private:

    FlyCallBack flyCallBack_;
    RunCallBack runCallBack_;


};

class Bar {

public:

    Bar(SwimCallBack swimCallBack, FlyCallBack flyCallBack, RunCallBack runCallBack) : swimCallBack_(swimCallBack),
                                                                                       flyCallBack_(flyCallBack),
                                                                                       runCallBack_(runCallBack) {

    }


    void doAction() {

        swimCallBack_();
        flyCallBack_();
        runCallBack_();

    };


private:


    SwimCallBack swimCallBack_;
    FlyCallBack flyCallBack_;
    RunCallBack runCallBack_;


};


int main() {
    Penguin p;
    Sparrow s;
    Foo foo(boost::bind(&Sparrow::fly, &s), boost::bind(&Penguin::run, &p));
    foo.doAction();
    Bar bar(boost::bind(&Penguin::swim, &p), boost::bind(&Sparrow::fly, &s), boost::bind(&Sparrow::run, &s));
    bar.doAction();


}