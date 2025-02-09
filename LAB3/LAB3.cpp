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
bool *shm_ptr;
bool activity=0;
int my_stdin=dup(STDIN_FILENO);

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
    //fork error checking
    if (pid == -1) {
        perror("fork failed");
        exit(1);
    }

    if (pid > 0) { // Parent process
        fflush(stdout);
        char input[256];
        while (1) {
            
            activity = 0;
            printf("> ");
            fflush(stdin);

            read(STDIN_FILENO, input, sizeof(input)); // Read message from pipe
            input[strcspn(input, "\n")] = '\0'; 
            if (strcmp(input, "quit") == 0) {
                break;
            }
            
            printf("here"); 
            printf("!"); 
            printf("%s",input); 
            printf("!\n"); 
            fflush(stdin);  
            //signal(pid,signal_func);
            activity=1;
            dup2(my_stdin,STDIN_FILENO);

        }

        kill(pid, SIGKILL);
        wait(0); 
    } else { // Child process
        //close(fd[0]); // Close read end of the pipe

        while (1) {
            sleep(10);
            if (activity == 1) {
                kill(pid, SIGUSR1);

            }
            if (activity == 0) {
                write( fd[1], "Inactivity detected!\n", 256); 
                kill(getppid(), SIGUSR1); // Signal parent
            }
        }
    }

 
    munmap(shm_ptr, sizeof(shm_ptr));
    return 0;
}
