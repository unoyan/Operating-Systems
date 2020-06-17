#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/user.h>
 #include <signal.h>

int main(void)
{
    
        pid_t pid;
        time_t rawtime;
        struct tm * timeinfo;
        
        pid = fork();

        //printf("The value of pid %d\n", pid);

        if (pid == 0) { /* child process */
            while(1){
                printf("Child process id %d\n", getpid());
                time ( &rawtime );
                timeinfo = localtime ( &rawtime );
                timeinfo = localtime ( &rawtime );
                printf ( "Current local time and date: %s", asctime (timeinfo) );
                sleep(1);
            }
            
        }
        else if (pid > 0) { /* parent process */
            //wait(NULL);
            sleep(5);
            printf("Parent process id %d\n", getpid());
            kill(pid, SIGKILL);
            printf("Child process id %d killed. \n", pid);
            return 0;
        }
    
  return 0;
}
