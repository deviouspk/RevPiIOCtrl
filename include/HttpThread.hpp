#include <onposix/AbstractThread.hpp>

#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

#include "RevPiIOCtrl.hpp"


#ifndef HTTPTHREAD_H
#define HTTPTHREAD_H

using namespace onposix;

class HttpThread : public AbstractThread {
    private:
    protected:
        RestClient::Connection* HttpConnection;
    public:
        virtual int HandleRequest() = 0;
        HttpThread();
};

#endif
    