#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

#include "HttpThread.hpp"

using namespace onposix;
using namespace std;

class Authentication : public HttpThread {
private:
    static int Id;
    static string Token;
    int ParseResponse(RestClient::Response response);
    int HandleRequest();
    
public:
    Authentication();
    
    void run();

    static int GetId();
    static string GetToken();
    static void Update(int id, string token);
};