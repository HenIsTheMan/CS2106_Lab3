#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define NUM_PROCESSES 5

int main() {

    int i, j, pid;

    //* Creating shared var "turn" and setting it to 0
    int* turn;
    int shmId;

    shmId = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0600);
    turn = shmat(shmId, NULL, 0);

    turn[0] = 0; //*turn = 0; also can
    //*/
    
    for(i=0; i<NUM_PROCESSES; i++)
    {
        if((pid = fork()) == 0) {
            while(turn[0] != i); //Busy waiting

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

        turn[0] = turn[0] + 1; //(void)++(*turn); also can
    }
    else {
        for(i=0; i<NUM_PROCESSES; i++) 
            wait(NULL);

        shmdt(turn); //Detach shm segment (located at specified address) from calling process's address space
        shmctl(shmId, IPC_RMID, 0); //Free shm
    }

}

