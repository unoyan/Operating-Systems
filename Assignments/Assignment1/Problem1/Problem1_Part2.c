// CPP code to create three child
// process of a parent
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/user.h>
 #include <signal.h>
// Driver code
int main()
{
    time_t rawtime;
    struct tm * timeinfo;
    int pid, pid1, pid2, pid3;

    // variable pid will store the
    // value returned from fork() system call
    pid = fork();

    // If fork() returns zero then it
    // means it is child process.
    if (pid == 0) {

        // First child needs to be printed
        // later hence this process is made
        // to sleep for 3 seconds.
//        sleep(3);

        // This is first child process
        // getpid() gives the process
        // id and getppid() gives the
        // parent id of that process.
//        printf("child[1] --> pid = %d and ppid = %d\n",
//            getpid(), getppid());
        
        while(1){
            printf("Child process id %d\n", getpid());
            time ( &rawtime );
            timeinfo = localtime ( &rawtime );
            timeinfo = localtime ( &rawtime );
            printf ( "Current local time and date: %s", asctime (timeinfo) );
            sleep(1);
        }
    }

    else {
        pid1 = fork();
        if (pid1 == 0) {
           while(1){
                printf("Child process id %d\n", getpid());
                time ( &rawtime );
                timeinfo = localtime ( &rawtime );
                timeinfo = localtime ( &rawtime );
                printf ( "Current local time and date: %s", asctime (timeinfo) );
                sleep(1);
            }
        }
        else {
            pid2 = fork();
            if (pid2 == 0) {
                // This is third child which is
                // needed to be printed first.
                while(1){
                    printf("Child process id %d\n", getpid());
                    time ( &rawtime );
                    timeinfo = localtime ( &rawtime );
                    timeinfo = localtime ( &rawtime );
                    printf ( "Current local time and date: %s", asctime (timeinfo) );
                    sleep(1);
                }
            }
            pid3 = fork();
            if (pid3 == 0) {
                // This is third child which is
                // needed to be printed first.
                while(1){
                    printf("Child process id %d\n", getpid());
                    time ( &rawtime );
                    timeinfo = localtime ( &rawtime );
                    timeinfo = localtime ( &rawtime );
                    printf ( "Current local time and date: %s", asctime (timeinfo) );
                    sleep(1);
                }
            }
            

            // If value returned from fork()
            // in not zero and >0 that means
            // this is parent process.
            else {
                // This is asked to be printed at last
                // hence made to sleep for 3 seconds.
                sleep(5);
                printf("Parent process id %d\n", getpid());
    //            for (int k=0; k<4; k++) {
    //                printf("Child process id %d killed. \n", pids[k]);
    //            }
                kill(pid,SIGKILL);
                kill(pid1,SIGKILL);
                kill(pid2,SIGKILL);
                kill(pid3,SIGKILL);
                printf("Child process id %d killed. \n", pid);
                printf("Child process id %d killed. \n", pid1);
                printf("Child process id %d killed. \n", pid2);
                printf("Child process id %d killed. \n", pid3);
                return 0;
            }
        }
    }

    return 0;
}

