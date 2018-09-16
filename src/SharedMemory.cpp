#include "semaphores.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>
#include "semaphores.h"
#include "SharedMemory.h"

#include <iostream>
using namespace std;

SharedMemory::SharedMemory(Semaphore* semIn_ds, Semaphore* semOut_ds, Message* msgIn_ds, Message* msgOut_ds)
{
    this->semIn_ds = semIn_ds;
    this->semOut_ds = semOut_ds;
    this->msgIn_ds = msgIn_ds;
    this->msgOut_ds = msgOut_ds;
}

SharedMemory::~SharedMemory()
{
    cout << "Shared Memory destroyed" << endl;
}
