#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

int fd[2];
pid_t pid;
pid_t par_pid;
bool *shm_ptr;
bool activity=0;

void signal_func(int sig) {
    dup2(fd[0],STDIN_FILENO);
}
void activity_set(bool activity) {
    activity=1;
}

int main() {
   

    if (pipe(fd) == -1) {
        perror("pipe failed");
    }
    signal(SIGUSR1,signal_func);
    // Create shared memory
    shm_ptr = (bool*)mmap(NULL,1, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    pid = fork();
    par_pid=getpid();
    //fork error checking
    if (pid == -1) {
        perror("fork failed");
        exit(1);
    }

    if (pid > 0) { // Parent process
        //close(fd[1]);  
        //char buffer[256];
        //printf("\n%s> ", buffer); // Print the message
        fflush(stdout);

        char input[256];
        while (1) {
            activity = 0;
            printf("> ");
            fflush(stdout);

            read(STDIN_FILENO, input, sizeof(input)); // Read message from pipe
            input[strcspn(input, "\n")] = '\0'; 
            if (strcmp(input, "quit") == 0) {
                break;
            }
             printf("here"); 
            printf("!%s!\n", input); 
            //send signal to the child
            //signal(pid,signal_func);
            activity=1;

        }

        kill(pid, SIGKILL);
        wait(0); 
    } else { // Child process
        //close(fd[0]); // Close read end of the pipe

        while (1) {
            sleep(10);
            if (activity == 1) {//need activity change here instead

            


            }
            if (activity == 0) {
                write( fd[1], "Inactivity detected!\n", 256); 
                kill(getppid(), SIGUSR1); // Signal parent
            }
        }
    }

 
    //munmap(shm_ptr, sizeof());
    return 0;
}
