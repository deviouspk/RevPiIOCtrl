#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

#include "HttpThread.hpp"

using namespace onposix;

class Authentication : public HttpThread {
private:
    static int Id;
    static int Key;
    int ParseResponse(RestClient::Response response);
    int HandleRequest();
    
public:
    Authentication();
    
    void run();

    static int GetId();
    static int GetKey();
    static void Update(int id, int key);
};