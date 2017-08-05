#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

#include "HttpThread.hpp"

using namespace onposix;

class CentralProcessing : public HttpThread { 
    private: 

        int ParseResponse(RestClient::Response response);
        int HandleRequest();
        std::string url;
        std::string body;

        long long CurrentTimeInMilliseconds();

    public:
        CentralProcessing();
        ~CentralProcessing() {};
        void run();
};