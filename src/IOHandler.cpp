#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>


#include "ThreadSynchronization.hpp"
#include "IOHandler.hpp"
#include <piControlIf.hpp>
#include <piControl.h>

piControl IOHandler::piCtrl;

bool IOHandler::GetIO(const char* pszVariableName) {
    int rc=-1;
    SPIVariable sPiVariable;
    SPIValue sPIValue;
    uint8_t i8uValue;
    uint16_t i16uValue;
    uint32_t i32uValue;

    strncpy(sPiVariable.strVarName, pszVariableName, sizeof(sPiVariable.strVarName));

    ThreadSynchronization::IOMutex.lock();
    rc = piCtrl.GetVariableInfo(&sPiVariable);

    if (rc < 0) {
        syslog(LOG_ERR, "IOHandler: Cannot find variable '%s'\n", pszVariableName);
        ThreadSynchronization::IOMutex.unlock();
        return -1;
    }; // else syslog(LOG_DEBUG, "IOHandler: SetIO(%s) [offset=%d length=%d bit=%d value=%d]\n", sPiVariable.strVarName, sPiVariable.i16uAddress, sPiVariable.i16uLength, sPiVariable.i8uBit, value);

    sPIValue.i16uAddress = sPiVariable.i16uAddress;
    sPIValue.i8uBit = sPiVariable.i8uBit;    

    rc = piCtrl.GetBitValue(&sPIValue);
    ThreadSynchronization::IOMutex.unlock();
    
    if (rc < 0) syslog(LOG_ERR, "IOHandler: Get bit error\n");
    else {
        syslog(LOG_DEBUG, "IOHandler: GetIO(%s) [offset=%d length=%d bit=%d value=%d]\n", sPiVariable.strVarName, sPiVariable.i16uAddress, sPiVariable.i16uLength, sPIValue.i8uBit, sPIValue.i8uValue);
        if (sPIValue.i8uValue==0) return false;
        else return true;
    }
}

int IOHandler::SetIO(const char* pszVariableName, bool on) {
    int rc=-1;
    SPIVariable sPiVariable;
    SPIValue sPIValue;
   
    strncpy(sPiVariable.strVarName, pszVariableName, sizeof(sPiVariable.strVarName));

    ThreadSynchronization::IOMutex.lock();
    rc = piCtrl.GetVariableInfo(&sPiVariable);

    if (rc < 0) {
        syslog(LOG_ERR, "IOHandler: Cannot find variable '%s'\n", pszVariableName);
        ThreadSynchronization::IOMutex.unlock();
        return -1;
    }; // else syslog(LOG_DEBUG, "IOHandler: SetIO(%s) [offset=%d length=%d bit=%d value=%d]\n", sPiVariable.strVarName, sPiVariable.i16uAddress, sPiVariable.i16uLength, sPiVariable.i8uBit, value);

    sPIValue.i16uAddress = sPiVariable.i16uAddress;
    sPIValue.i8uBit = sPiVariable.i8uBit;    
    if (on) sPIValue.i8uValue = 1; else sPIValue.i8uValue = 0;

    // Set bit
    rc = piCtrl.SetBitValue(&sPIValue);
    if (rc < 0) syslog(LOG_ERR, "IOHandler: Set bit error\n");
    else syslog(LOG_DEBUG, "IOHandler: SetIO(%s) [offset=%d length=%d bit=%d value=%d]\n", sPiVariable.strVarName, sPiVariable.i16uAddress, sPiVariable.i16uLength, sPiVariable.i8uBit, sPIValue.i8uValue);

    ThreadSynchronization::IOMutex.unlock();
    
    return rc;
}