#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <syslog.h>
#include <unistd.h>

//#define NDEBUG

#include <json/json.h>
#include <json/json-forwards.h>

#include <onposix/AbstractThread.hpp>
#include <onposix/Logger.hpp>
#include <onposix/PosixMutex.hpp>
#include <onposix/PosixSharedQueue.hpp>

#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

#include "RevPiIOCtrl.hpp"
#include "Authentication.hpp"
#include "CentralProcessing.hpp"
#include "IOHandler.hpp"

#include <piControlIf.hpp>
#include <piControl.h>

using namespace onposix;

PosixMutex IOMutex;

int main()
{
    // initialize RestClient
    RestClient::init();

    PosixMutex IOMutex;
    Authentication authentication;
    
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << "                    IO Controller                       " << std::endl;
    std::cout <<  "-------------------------------------------------------" << std::endl;

    std::cout << "Starting Tasks...\n";

    syslog(LOG_INFO, "--------------------------------------------------");
    syslog(LOG_INFO, "-----------------   IO Controller  ---------------");
    syslog(LOG_INFO, "--------------------------------------------------");
   
#if DYNAMIC_AUTHENTICATION
    std::cout << "Starting Athentication ..." << std::endl;
    syslog(LOG_INFO, "Starting Authentication ..");
    authentication.start();
    // wait for authentication to finish before going on
    authentication.waitForTermination();
#else
    std::cout << "Only static authentication." << std::endl;
    syslog(LOG_INFO, "Only static Authentication ..");
#endif
    // INPUTS
    syslog(LOG_INFO, "Checking all IOs ..");

    std::cout << "Finished, starting all other thtreads.." << std::endl;

    CentralProcessing centralProcessing;
    centralProcessing.start();
    // Wait for all tasks to finish but the application should not get here
    centralProcessing.waitForTermination();

    return 0;
}