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
    syslog(LOG_INFO, "EntranceLoopAct: %i",  IOHandler::GetIO("EntranceLoopAct"));
    syslog(LOG_INFO, "ExitLoopAct    : %i",  IOHandler::GetIO("ExitLoopAct"));
    syslog(LOG_INFO, "EntranceCO     : %i",  IOHandler::GetIO("EntranceCO"));
    syslog(LOG_INFO, "ExitCO         : %i",  IOHandler::GetIO("ExitCO"));
    syslog(LOG_INFO, "PLCAuto        : %i",  IOHandler::GetIO("PLCAuto"));
    syslog(LOG_INFO, "TicketAct      : %i",  IOHandler::GetIO("TicketAct"));
    syslog(LOG_INFO, "MoneyAct       : %i",  IOHandler::GetIO("MoneyAct"));
    // OUPUTS
    syslog(LOG_INFO, "Continu12V     :   "); IOHandler::SetIO("Continu12V", true);
    sleep(1); IOHandler::SetIO("Continu12V", false);
    syslog(LOG_INFO, "ContinuClosed  :   "); IOHandler::SetIO("ContinuClosed", true);
    sleep(1); IOHandler::SetIO("ContinuClosed", false);
    syslog(LOG_INFO, "OpenEntrance   :   "); IOHandler::SetIO("OpenEntrance", true);
    sleep(1); IOHandler::SetIO("OpenEntrance", false);
    syslog(LOG_INFO, "OpenExit       :   "); IOHandler::SetIO("OpenExit", true);
    sleep(1); IOHandler::SetIO("OpenExit", false);

    std::cout << "Finished, starting all other thtreads.." << std::endl;

    CentralProcessing centralProcessing;
    centralProcessing.start();
    // Wait for all tasks to finish but the application should not get here
    centralProcessing.waitForTermination();

    return 0;
}