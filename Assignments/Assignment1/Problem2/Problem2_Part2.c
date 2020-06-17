#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>
#include <signal.h>
#include <sys/user.h>
int main()
{
    // We use three pipes
    // First pipe to send and receive input integer from parent
    // Second pipe to send and receive input integer from child
    // Third pipe to send and receive input integer from grandchild

    int fd1[2]; // Used to store two ends of first pipe
    int fd2[2]; // Used to store two ends of second pipe
    int fd3[2]; // Used to store two ends of third pipe

    int input_int[1];
    pid_t pid, pid1;

    if (pipe(fd1)==-1)
    {
        fprintf(stderr, "Pipe Failed" );
        return 1;
    }
    if (pipe(fd2)==-1)
    {
        fprintf(stderr, "Pipe Failed" );
        return 1;
    }
    if (pipe(fd3)==-1)
    {
        fprintf(stderr, "Pipe Failed" );
        return 1;
    }

    pid = fork();
    pid1 = getpid();
    //printf(" Pid : %d\n", pid1);


    // Parent process
    if (pid > 0)
    {
        sleep(5);
        //char concat_str[100];
        printf("Please enter a input number: ");
        scanf("%d", input_int);
        printf("------------------o----------------\n");
        printf("Parent with PID %d: Input %d \n",getpid(), input_int[0]);
        // Write input string and close writing end of first
        // pipe.
        sleep(5);
        write(fd1[1], input_int, sizeof(input_int)+1);
        
        close(fd1[1]);
        
        // Wait for child to send a integer
        wait(NULL);

        close(fd2[1]); // Close writing end of second pipe
        close(fd2[0]);
        // Read string from child, print it and close reading end.
                    sleep(5);

        read(fd1[0], input_int, sizeof(input_int)+1);
        sleep(5);
        printf("Returned from children input becomes %d\n", input_int[0]);
        close(fd1[0]); // Close reading end of first pipe

        //signal(SIGQUIT, SIG_IGN);
        //kill(-getpid(), SIGQUIT);
        //exit(0);
    }

    // child process
    else if (pid == 0)
    {
        //printf("Child process id %d\n", getpid());
        // Read a string using first pipe
       
        pid1= fork();
        if(pid1 == 0){
            sleep(5);
            //printf ("Grandchild with PID %d and PPID %d.\n", getpid (), getppid ());
            // Read a string using first pipe
            read(fd3[0], input_int, sizeof(input_int)+1);
            int i = input_int[0];
            int o = i*2;
            printf("Grandchild with PID %d: Input %d, Output %d \n",getpid(), i, o);
            input_int[0] = o;
            // Write input_int and close writing end
            sleep(5);

            write(fd2[1], input_int, sizeof(input_int)+1);
            close(fd2[1]);
        }else if(pid1>0){
           sleep(5);

           read(fd1[0], input_int, sizeof(input_int)+1);
           
           int i = input_int[0];
           int o = i*2;
           printf("Child with PID %d: Input %d, Output %d \n",getpid(), i, o);
           input_int[0] = o;
           sleep(5);

            // Write concatenated string and close writing end
           write(fd3[1], input_int, sizeof(input_int)+1);
            sleep(5);

            wait(NULL);
           // Read a input_int using second pipe
            read(fd2[0], input_int, sizeof(input_int)+1);
            //i = input_int[0];
            //o = i*2;
            //printf("Child with PID %d returned from grandchild: Input %d, Output %d \n", getpid(), i, o);
            //input_int[0] = i;
            // Write concatenated string and close writing end
            write(fd1[1], input_int, sizeof(input_int)+1);
            close(fd1[1]);
            close(fd3[1]);
        }
        
        
       
    }
}
