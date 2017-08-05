#include <stdio.h>
#include <sys/sem.h>
#include <syslog.h>

#include "ThreadSynchronization.hpp"

using namespace onposix;

PosixMutex ThreadSynchronization::SerialMutex;
PosixMutex ThreadSynchronization::SettingsMutex;
PosixMutex ThreadSynchronization::IOMutex;
PosixMutex ThreadSynchronization::IdKeyMutex;