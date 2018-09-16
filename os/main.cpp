#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fstream>
#include "semaphores.h"
#include "SharedMemory.h"
#include "stdlib.h"

#define NUMBER_OF_PROCESSES 3

using namespace std;

void createShm(int& id, void** shmAdress, int size);
void deleteShm(int id, void* shmAdress);
void capitalizeString(char* str, int size);
int sizeOfFile(const string& inFileName);
string randomLine(const string& inFileName, int FileSize);
void Producer(Semaphore& semWriteIn_ds, Semaphore& semReadIn_ds, Semaphore& semWriteOut_ds, Semaphore& semReadOut_ds, Semaphore& semTotalMatch, Semaphore& semCoutMutex, Message* shmIn_ds, Message* shmOut_ds, int* totalMatch, const string& inFile);
void Consumer(Semaphore& semWriteIn_ds, Semaphore& semReadIn_ds, Semaphore& semWriteOut_ds, Semaphore& semReadOut_ds, Message* shmIn_ds, Message* shmOut_ds, int loops, int noProducers);

void ChildProcess(SharedMemory *sharedMemory);
void ParentProcess(SharedMemory **sharedMemoryArray);

int main (int argc, char *argv[])
{
    ifstream inFile;
    string inFileName="";
    Message* msgIn_ds;
    Message* msgOut_ds;
    int* totalMatch;
    int N =5;
    int loops = 1000;
    int shInFile, shm_totalMatch;
		int shmIn_ds[3], shmOut_ds[3];
    int status;
		SharedMemory* sharedMemoryArray[3];
    pid_t pid, consumerPid;

    for(int i =0; i < NUMBER_OF_PROCESSES; i++) //katasteuazoume ta producer process
    {
				Semaphore* semIn_ds = new Semaphore(IPC_PRIVATE, 1);
				Semaphore* semOut_ds = new Semaphore(IPC_PRIVATE, 1);
				createShm(shmIn_ds[i],(void**) &msgIn_ds, sizeof(Message));
				createShm(shmOut_ds[i],(void**)&msgOut_ds, sizeof(Message));
				SharedMemory* sharedMemory = new SharedMemory(semIn_ds, semOut_ds, msgIn_ds, msgOut_ds);
				sharedMemoryArray[i] = sharedMemory;
				cout << "Create shared memory" << endl;
        pid = fork();
				if(pid == 0) {
					ChildProcess(sharedMemory);
					exit(1);
        }
    }

    ParentProcess(sharedMemoryArray);

		cout << "Parent" << endl;
		int exitValue;

    while((pid = wait(&status)) > 0);

    //delete kai detach oti emeine
    //ta semaphore diagrafontai apo to destructor tou Semaphore class
		for (int i=0; i < NUMBER_OF_PROCESSES; i++) {
			SharedMemory* sharedMemory = sharedMemoryArray[i];
	    deleteShm(shmIn_ds[i], sharedMemory->msgIn_ds);
	    deleteShm(shmOut_ds[i], sharedMemory->msgOut_ds);
		}
}

//returns size of array to save file
int sizeOfFile(const string& inFileName)
{
    int i =0;
    string line;
    ifstream inFile(inFileName.c_str());

    while(getline(inFile, line))
    {
        i++;
    }

    inFile.close();

    return i;
}

//returns random line
string randomLine(const string& inFileName, int FileSize)
{
    string line;
    int randNum = rand() %FileSize+1; // arithmos apo [1,FileSize]
    ifstream inFile(inFileName.c_str());

    for(int i=0; i <randNum; i++)
    {
        getline(inFile, line);
    }

    inFile.close();
    return line; //epistrefei random line
}

//for child processes
void ChildProcess(SharedMemory *sharedMemory) {
		cout << "Child Process " << endl;
    Message tmpMessage;
    int pidMatch = 0;

    tmpMessage.pid = getpid();

    while (true) {

        (sharedMemory->semIn_ds)->down(); //katevazoume to semaphore gia write sto in_ds
        (sharedMemory->msgIn_ds)->pid = tmpMessage.pid;
        strcpy((sharedMemory->msgIn_ds)->msg, "Hello");
        cout << "Process " << tmpMessage.pid << ": Writing message...\n";
        (sharedMemory->semIn_ds)->up(); //shkwnoume to semaphore gia read apo in_ds

        (sharedMemory->semOut_ds)->down(); //katevazoume to semaphore gia read apo out_ds
        tmpMessage.pid = (sharedMemory->msgOut_ds)->pid; //diavazoume ta dedomena
        strcpy(tmpMessage.msg, (sharedMemory->msgOut_ds)->msg);
        cout << "Process " << tmpMessage.pid << ": Reading message: " << tmpMessage.msg << endl;
        (sharedMemory->semOut_ds)->up();
        sleep(1);
    }

    //detach shared memory segments
    // shmdt(sharedMemory->msgIn_ds);
    // shmdt(sharedMemory->msgOut_ds);
}

//for parent processes
void ParentProcess(SharedMemory **sharedMemoryArray) {
		cout << "Parent Process" << endl;
    Message tmpMessage;

    while (true) {
        for (int i = 0; i < NUMBER_OF_PROCESSES; i++) {
            SharedMemory* sharedMemory = sharedMemoryArray[i];

            (sharedMemory->semIn_ds)->down(); //katevazoume to semaphore gia write sto in_ds
            tmpMessage.pid = (sharedMemory->msgIn_ds)->pid; //antigrafoume to mnm
            strcpy(tmpMessage.msg, (sharedMemory->msgIn_ds)->msg);
            cout << "Reading... " << tmpMessage.msg << endl;
            (sharedMemory->semIn_ds)->up(); //shkwnoume to semaphore gia read apo in_ds

            (sharedMemory->semOut_ds)->down(); //katevazoume to semaphore gia write sto in_ds
            (sharedMemory->msgOut_ds)->pid = tmpMessage.pid;
            strcpy((sharedMemory->msgOut_ds)->msg, strcat(tmpMessage.msg, "ooooooooooo"));
            cout << "Process " << tmpMessage.pid << ": Writing message...\n";
            (sharedMemory->semOut_ds)->up(); //shkwnoume to semaphore gia read apo in_ds
            sleep(1);
        }
    }
}

void Producer(Semaphore& semWriteIn_ds, Semaphore& semReadIn_ds, Semaphore& semWriteOut_ds, Semaphore& semReadOut_ds, Semaphore& semTotalMatch, Semaphore& semCoutMutex, Message* shmIn_ds, Message* shmOut_ds, int* totalMatch, const string& inFile)
{
    Message tmpMessage;
    int pidMatch = 0;
    int fileSize = sizeOfFile(inFile);

    srand(time(NULL)^ getpid());

    while(true)
    {
        strcpy(tmpMessage.msg, randomLine(inFile, fileSize).c_str()); //pairnoume ena random line apo arxeio

        tmpMessage.pid = getpid();

        semWriteIn_ds.down(); //katevazoume to semaphore gia write sto in_ds
        shmIn_ds->pid = tmpMessage.pid;
        strcpy(shmIn_ds->msg, tmpMessage.msg);
        semReadIn_ds.up(); //shkwnoume to semaphore gia read apo in_ds

        semReadOut_ds.down(); //katevazoume to semaphore gia read apo out_ds
        tmpMessage.pid = shmOut_ds->pid; //diavazoume ta dedomena
        strcpy(tmpMessage.msg, shmOut_ds->msg);
        semWriteOut_ds.up();

        if(tmpMessage.pid == -1) //minima telous, oti teleiwse o consumer
        {
            break;
        }

        if(getpid() == tmpMessage.pid) //exoume match creator kai receiver
        {
            pidMatch++;
        }
        else
        {
            semCoutMutex.down(); //apoklistikh prosvash sto stdout gia na mhn mperdeuontai ta minimata

            cout<<"Creator process pid:  "<<tmpMessage.pid<<endl;
            cout<<"Receiver process pid: "<<getpid()<<endl;
            cout<<"Message:              "<<tmpMessage.msg<<endl<<endl<<flush;

            semCoutMutex.up();
        }
    }

    semTotalMatch.down(); //grafoume ta total match
    *totalMatch += pidMatch;
    semTotalMatch.up();

    //detach shared memory segments
    shmdt(shmIn_ds);
    shmdt(shmOut_ds);
    shmdt(totalMatch);

    exit(pidMatch);
}

void Consumer(Semaphore& semWriteIn_ds, Semaphore& semReadIn_ds, Semaphore& semWriteOut_ds, Semaphore& semReadOut_ds, Message* shmIn_ds, Message* shmOut_ds, int loops, int noProducers)
{
    Message tmpMessage;

    for(int i=0; i < loops; i++)
    {

        semReadIn_ds.down(); //katevazoume ton semaphore gia read apo in_ds

        tmpMessage.pid = shmIn_ds->pid; //antigrafoume to mnm
        strcpy(tmpMessage.msg, shmIn_ds->msg);

        semWriteIn_ds.up(); //sikwnoume to semaphore gia write apo producer

        capitalizeString(tmpMessage.msg, strlen(tmpMessage.msg));

        semWriteOut_ds.down(); //katevazoume to semaphore gia write

        shmOut_ds->pid = tmpMessage.pid; //antigrafoume to mnm sto shared memory
        strcpy(shmOut_ds->msg, tmpMessage.msg);

        semReadOut_ds.up();

    }

    //broadcast message to stop processes
    for(int i =0; i< noProducers; i++)
    {
        semWriteIn_ds.up();//dn theloume kapoios producer na prospathei na grapsei enw exei teleiwse o consumer

        semWriteOut_ds.down(); //grafoume ena ena to mnm telous kai perimenoume enan enan tous producers na to diavasoun
        shmOut_ds->pid = -1;
        semReadOut_ds.up();
    }

    shmdt(shmIn_ds);
    shmdt(shmOut_ds);

    exit(loops);
}

void capitalizeString(char* str, int size)
{
    for(int i =0; i < size; i++)
    {
        if(str[i]<'a' || str[i]>'z') continue; //ama dn einai mikros xaraktiras agnwise ton

        str[i] -= 'a'-'A';
    }
}

//creates shared memory segment
void createShm(int& id, void** shmAdress, int size)
{
    if((id = shmget(IPC_PRIVATE, size, IPC_CREAT|0660))==-1)
    {
        perror("Shmget");
        exit(1);
    }

    if((*shmAdress = (Message*)shmat(id, 0, 0)) ==(void*)-1)
    {
        perror("Shmat");
        exit(1);
    }
}

//detach and delete shared memory segment
void deleteShm(int id, void* shmAdress)
{
    if(shmdt(shmAdress)==-1)
    {
        perror("Shared Memory detached parent");
        exit(1);
    }

    if(shmctl(id, IPC_RMID, 0)==-1)
    {
        perror("Shared Memory remove");
    }
}
