#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <syslog.h>

#include <json/json.h>
#include <json/json-forwards.h>

#include "RevPiIOCtrl.hpp"
#include "HttpThread.hpp"

HttpThread::HttpThread() {
    // get a connection object
    HttpConnection = new RestClient::Connection(SERVER_ADDRESS);
}