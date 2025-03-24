#include "barrier.h"

#include <stdio.h>
#include <sys/shm.h>
#include <semaphore.h>

static int processTotalCount; //nproc (ignored = 0 from L03Qns.pdf, not updating after init so no need shm)

static int processCurrCountShmId;
static int* processCurrCountPtr; //count (ignored = 0 from L03Qns.pdf)

static int mutexShmId;
static sem_t* mutexPtr;

static int turnstile0ShmId;
static sem_t* turnstile0Ptr;

static int turnstile1ShmId;
static sem_t* turnstile1Ptr;

//* Technically is initializing more than barrier but oh wells (just following L03Qns.pdf)
void init_barrier(int num_proc) { //"num_proc" (just following L03Qns.pdf)
    processTotalCount = num_proc;

    processCurrCountShmId = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0600);

    processCurrCountPtr = shmat(processCurrCountShmId, NULL, 0);

    *processCurrCountPtr = 0; //Init to 0

    mutexShmId = shmget(IPC_PRIVATE, sizeof(sem_t), IPC_CREAT | 0600);

    mutexPtr = (sem_t*)shmat(mutexShmId, NULL, 0);

    sem_init(mutexPtr, 1, 1); //Init to 1

    turnstile0ShmId = shmget(IPC_PRIVATE, sizeof(sem_t), IPC_CREAT | 0600);

    turnstile0Ptr = (sem_t*)shmat(turnstile0ShmId, NULL, 0);

    sem_init(turnstile0Ptr, 1, 0); //Init to 0

    turnstile1ShmId = shmget(IPC_PRIVATE, sizeof(sem_t), IPC_CREAT | 0600);

    turnstile1Ptr = (sem_t*)shmat(turnstile1ShmId, NULL, 0);

    sem_init(turnstile1Ptr, 1, 1); //Init to 1
}
//*/

void reach_barrier() { //2 stages to allow for reusability
    //* Stage 0
    sem_wait(mutexPtr);

    ++(*processCurrCountPtr);

    if(*processCurrCountPtr == processTotalCount){ //Last process
        sem_wait(turnstile1Ptr);
        sem_post(turnstile0Ptr);
    }

    sem_post(mutexPtr);

    sem_wait(turnstile0Ptr);
    sem_post(turnstile0Ptr);
    //*/

    //* Stage 1
    sem_wait(mutexPtr);

    --(*processCurrCountPtr);

    if(*processCurrCountPtr == 0){ //Another last process (same or diff as before)
        sem_wait(turnstile0Ptr);
        sem_post(turnstile1Ptr);
    }

    sem_post(mutexPtr);

    sem_wait(turnstile1Ptr);
    sem_post(turnstile1Ptr);
    //*/
}

void destroy_barrier(int my_pid) {
    if(my_pid != 0) { //Technically no need to check since alr done in main()
        // Destroy the semaphores and detach
        // and free any shared memory. Notice
        // that we explicity check that it is
        // the parent doing it.

        shmdt(processCurrCountPtr);

        shmctl(processCurrCountShmId, IPC_RMID, 0);

        sem_destroy(mutexPtr);

        shmdt(mutexPtr);

        shmctl(mutexShmId, IPC_RMID, 0);

        sem_destroy(turnstile0Ptr);

        shmdt(turnstile0Ptr);

        shmctl(turnstile0ShmId, IPC_RMID, 0);

        sem_destroy(turnstile1Ptr);

        shmdt(turnstile1Ptr);

        shmctl(turnstile1ShmId, IPC_RMID, 0);
    }
}


