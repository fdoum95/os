#ifndef Included_SharedMemory_H
#define Included_SharedMemory_H

#include "semaphores.h"

struct Message
{
  pid_t pid;
  char msg[1024];
};

class SharedMemory
{
    public:
      	Semaphore* semIn_ds;
      	Semaphore* semOut_ds;
        Message* msgIn_ds;
        Message* msgOut_ds;

    public:
        SharedMemory(Semaphore* semIn_ds, Semaphore* semOut_ds, Message* msgIn_ds, Message* msgOut_ds);
        ~SharedMemory();
};

#endif
