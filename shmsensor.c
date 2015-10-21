#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/shm.h>

void sigalrm_parent_handler(int sig);

struct shared_use_st {
    int sense_data;
};

struct shared_use_st *shared_stuff;

int shmid;

int main(){

    
    void *shared_memory = (void *)0;
    int threshold = 25;
    int running = 1;
    pid_t pid;
    if((shmid = shmget((key_t)1111, sizeof(struct shared_use_st), 0666|IPC_CREAT))==-1){
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }
    
    if((shared_memory = shmat(shmid, (void *)0,0))==((void *)-1)){
        fprintf(stderr,"shmat failed\n");
        exit(EXIT_FAILURE);
    }
    
    shared_stuff = (struct shared_use_st *)shared_memory;
    
    pid = fork();
    switch(pid){
        case -1:
            fprintf(stderr,"fork failed\n");
            exit(EXIT_FAILURE);
            break;
        case 0: //sensor (child)
    
            printf("Memory attached at %X\n", (int)shared_memory);
    
            
            while(running){
                shared_stuff->sense_data = (rand() % 100) - 50;
                if(shared_stuff->sense_data > threshold){
                    printf("child rasing alarm with value %d\n",shared_stuff->sense_data);
                    kill(getppid(),SIGALRM);
                }else{
                    printf("value was %d\n",shared_stuff->sense_data);
                }
                sleep(2);
            }
            break;
        default:
            break;          
    }
    
    struct sigaction parent_action;
    parent_action.sa_handler = sigalrm_parent_handler;
    sigemptyset(&parent_action.sa_mask);
    parent_action.sa_flags = 0;
    sigaction(SIGALRM, &parent_action, 0);
    while(running){
    } 
}

void sigalrm_parent_handler(int sig){
    printf("interupt received\n");
    printf("Alarm Raised with value %d\n",shared_stuff->sense_data);
}

