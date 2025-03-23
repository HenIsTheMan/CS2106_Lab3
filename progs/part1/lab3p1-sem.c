#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <semaphore.h>

#define NUM_PROCESSES 5

int main() {
    int i, j, pid;

    //* Declarations
    sem_t* semaphoreArr[NUM_PROCESSES];
    int shmIdArr[NUM_PROCESSES];
    //*/

    //* Init everything 1st by parent process
    for(i = 0; i < NUM_PROCESSES; ++i){
        shmIdArr[i] = shmget(IPC_PRIVATE, sizeof(sem_t), IPC_CREAT | 0600);

        semaphoreArr[i] = (sem_t*)shmat(shmIdArr[i], NULL, 0);

        sem_init(semaphoreArr[i], 1, 0);
    }
    //*/
    
    for(i=0; i<NUM_PROCESSES; i++)
    {
        if((pid = fork()) == 0){
            sem_wait(semaphoreArr[i]); //Wait for parent to call sem_post(semaphoreArr[i]);

            break;
        }
    }

    if(pid == 0) {
        printf("I am child %d\n", i);

        for(j = i*10; j<i*10 + 10; j++){
            printf("%d ", j);
            fflush(stdout);
            usleep(250000);
        }

        printf("\n\n");
    }
    else {
        for(i = 0; i < NUM_PROCESSES; ++i){
            sem_post(semaphoreArr[i]);

            wait(NULL);
        }

        //* Deinit everything last by parent process
        for(i = 0; i < NUM_PROCESSES; ++i){
            sem_destroy(semaphoreArr[i]); //Destroy semaphore

            shmdt(semaphoreArr[i]); //Detach shm

            shmctl(shmIdArr[i], IPC_RMID, 0); //Free shm
        }
        //*/
    }
}

