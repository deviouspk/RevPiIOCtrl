#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <stdint.h>
#include <iostream>
#include <syslog.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>

#include <json/json.h>
#include <json/json-forwards.h>

#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

#include "CentralProcessing.hpp"
#include "IOHandler.hpp"
#include "HttpThread.hpp"
#include "Authentication.hpp"
#include "ThreadSynchronization.hpp"

using namespace onposix;

CentralProcessing::CentralProcessing() {
    
    // compose url
    char id[10];
    sprintf(id, "%d", Authentication::GetId());
    //url.append(BASE_API_PATH).append(id).append("/parking").append("/entrance");
   
    url.append("/imber/io");

    // set connection timeout to 5s
    HttpConnection->SetTimeout(5);

    // set headers
    RestClient::HeaderFields headers;
    headers["Content-Type"] = "text/json";
    headers["Key"] = Authentication::GetKey();
    HttpConnection->SetHeaders(headers);

    // compose body
    //char key[10];
    //sprintf(key, "%d", Authentication::GetKey());
    //body.append("{\"key\": ").append(key).append("\"}");
}

int CentralProcessing::HandleRequest() {
    RestClient::Response response;

    syslog(LOG_INFO, "IO: sending GET %s", url.c_str());
    response = HttpConnection->get(url);
    return ParseResponse(response);
}

long long CentralProcessing::CurrentTimeInMilliseconds() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // caculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}

int CentralProcessing::ParseResponse(RestClient::Response response) {
    
    Json::Value root;
    Json::Reader reader;
    int id;
    int key;
    bool O1;

    bool parsingSuccessful = reader.parse(response.body, root);
    syslog(LOG_INFO, "IO: response on GET settings= (%i) %s", response.code, response.body.c_str());
    
    if (parsingSuccessful)
    {
        /*
        if ((key=root.get("key" , 0).asInt())==0) { // defaults to 0 if not found in body
            syslog(LOG_DEBUG, "IO: key not found in response");
            return 0;
        }  
        if (key==Authentication::GetKey()) {
            */

            // parse all other parameters if key matches
            // Critical section (is not absolutely nessary because only one thread accesses the IO's)
            ThreadSynchronization::SettingsMutex.lock();


            // code to be adapted to json content
            if (!root.isMember("O1")) {
                syslog(LOG_DEBUG, "IO: parameter 'O1' not found in response");
            } else {
                O1=root.get("O1", false).asBool();
                IOHandler::SetIO("O1", O1);
            }

            // End critical section
            ThreadSynchronization::SettingsMutex.unlock();
            syslog(LOG_INFO, "IO: settings updated");
            return 1;
        /*
        } else syslog(LOG_DEBUG, "IO: authentication failed");
        */

    } else syslog(LOG_ERR, "IO: parsing failed");

    return 0;
}

void CentralProcessing::run() {
    
    syslog(LOG_INFO, "SETTINGS: thread started");

    while(1)  {
        HandleRequest();
        usleep(1000000); 
    }
    return;
}