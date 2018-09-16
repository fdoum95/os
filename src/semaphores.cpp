#include "semaphores.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>
using namespace std;

Semaphore::Semaphore(int key, int initialValue)
{
    if((id=semget(key, 1, IPC_CREAT|0660)) == -1)
    {
        perror("Semaphore create");
        exit(1);
    }

    //initialize semaphore
    semctl(id, 0, SETVAL, initialValue);
}

Semaphore::~Semaphore()
{
    cout<<"Semaphore destroyed"<<endl;
    if(semctl(id, 0, IPC_RMID, NULL) ==-1)
    {
        perror("Semaphore delete");
        exit(1);
    }
}

void Semaphore::down()
{
    sembuf downSem;

    downSem.sem_num = 0;
    downSem.sem_op = -1;
    downSem.sem_flg = 0;//SEM_UNDO; //mporei kati allo
    if(semop(id, &downSem, 1)==-1)
    {
        printf("Semop down\n");
    }
}

void Semaphore::up()
{
    sembuf upSem;

    upSem.sem_num = 0;
    upSem.sem_op = 1;
    upSem.sem_flg = 0;//SEM_UNDO; //mporei kati allo
    if(semop(id, &upSem, 1)==-1)
    {
        printf("Semop up\n");
    }

}
