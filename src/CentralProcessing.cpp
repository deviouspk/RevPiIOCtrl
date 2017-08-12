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
    //sprintf(id, "%d", Authentication::GetId());
    syslog(LOG_INFO, "IO: token=%s",Authentication::GetToken().c_str());
    url.append(BASE_API_PATH);

    // set connection timeout to 5s
    HttpConnection->SetTimeout(5);

    // set headers
    RestClient::HeaderFields headers;
    headers["Content-Type"] = "text/json";
    headers["Token"] = Authentication::GetToken();
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
    int output_id;
    bool output_value;
    char io_string[5];

    bool parsingSuccessful = reader.parse(response.body, root);
    //syslog(LOG_INFO, "IO: response on GET settings= (%i) %s", response.code, response.body.c_str());
    
    if (parsingSuccessful)
    {
            if (!root.isMember("outputs")) {
                syslog(LOG_DEBUG, "IO: no member 'outputs'' found in json");
                return 0;
            }
            const Json::Value outputs = root["outputs"];

            for (int index = 0; index < outputs.size(); ++index) {
                Json::Value output = outputs[index];
                
                if (output.isMember("output_id") && output.isMember("value")) {
                     output_id    = output.get("output_id", 0).asInt();
                     output_value = output.get("value", 0).asBool();
                     syslog(LOG_INFO, "IO: output_id=%d value=%d", output_id, (output_value ? 1 : 0));

                     if ((output_id>=0) && (output_id <32)) {
                        sprintf(io_string, "O_%d", output_id+1);
                        IOHandler::SetIO(io_string, output_value);
                     }
                } 
            }
    } else syslog(LOG_ERR, "IO: parsing failed");

    return 0;
}

void CentralProcessing::run() {
    
    syslog(LOG_INFO, "SETTINGS: thread started");

    while(1)  {
        HandleRequest();
        usleep(10000000); 
    }
    return;
}