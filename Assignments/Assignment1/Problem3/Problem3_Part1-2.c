#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <signal.h>
#include <sys/user.h>


//function to convert ascii char[] to hex-string (char[])
void string2hexString(char* input, char* output)
{
    int loop;
    int i;
    
    i=0;
    loop=0;
    
    while(input[loop] != '\0')
    {
        sprintf((char*)(output+i),"%02X", input[loop]);
        loop+=1;
        i+=2;
    }
    //insert NULL at the end of the output string
    output[i++] = '\0';
}
//int argc, char **argv

int main(void){
// We use three pipes
   // First pipe to send and receive input integer from parent
   // Second pipe to send and receive input integer from child
   // Third pipe to send and receive input integer from grandchild

    int fd[2]; // Used to store two ends of first pipe
    int fd1[2]; // Used to store two ends of second pipe
    const char *nameA = "Shared MemoryA";
    const char *nameB = "Shared MemoryB";
    long int SIZE = 0;
    long int pred_SIZE = 999999999999;
    int buf_SIZE_A = 0;
    int buf_SIZE_B = 0;
    int buf_SIZE = 4096;
    pid_t pidA, pidB;
    int shm_fdA,shm_fdB ;
    int status;
    char* message;
    /* Character variable to read the content of file */
    char c;
    if (pipe(fd)==-1)
    {
       fprintf(stderr, "Pipe Failed" );
       return 1;
    }
    if (pipe(fd1)==-1)
    {
       fprintf(stderr, "Pipe Failed" );
       return 1;
    }
    
    char *line = NULL;  /* forces getline to allocate with malloc */
    char* a = (char *) malloc(buf_SIZE);
    pidA = fork();
//    printf("Input file size is: %ld \n", SIZE);
    if (pidA > 0)
    {
        pidB = fork();
        //PARENT process
        if(pidB >0){
            size_t len = 0;     /* ignored when line = NULL */
            ssize_t read;
            while ((read = getline(&line, &len, stdin)) != -1) {
                if (read > 0){ 
                    //printf ("\n  Read %zd chars from stdin, allocated %zd bytes for line : \n %s\n", read, len, line);
                    //fflush(stdout);
                    //printf("SIZE = %ld \n", strlen(a));
                    //a+=strlen(line);
                    //printf(" a is %s \n", a);
                    if(SIZE+strlen(line)>buf_SIZE){
                        buf_SIZE += buf_SIZE;
                        a = (char *) realloc(a, buf_SIZE);
                    }

                    sprintf(a+SIZE, "%s", line);

                    SIZE+=strlen(line);

                }
            }
            a = (char *) realloc(a, SIZE);
            write(fd[1], &SIZE, sizeof(SIZE)+1);
            write(fd1[1], &SIZE, sizeof(SIZE)+1);
            
            while (buf_SIZE_A < SIZE) {
                //printf ("\n  read %zd chars from stdin, allocated %zd bytes for line : \n %s\n", read, len, line);
                //printf("%c", (char)*(a+buf_SIZE_A));
                c = (char)*(a+buf_SIZE_A); 
                //printf("%c", c); 
                write(fd[1], &c, sizeof(c)+1);
                buf_SIZE_A+=sizeof(c);
            }
            while (buf_SIZE_B < SIZE) {
                //printf ("\n  read %zd chars from stdin, allocated %zd bytes for line : \n %s\n", read, len, line);
                //printf("%c", (char)*(a+buf_SIZE_A));
                c = (char)*(a+buf_SIZE_A); 
                //printf("%c", c); 
                write(fd1[1], &c, sizeof(c)+1);
                buf_SIZE_B+=sizeof(c);
            }
            close(fd[1]);
            close(fd1[1]);
            //printf("\n"); 
            //free (line);  /* free memory allocated by getline */
            /* open the shared memory segment A */
            shm_fdA = shm_open(nameA, O_RDONLY, 0666);
            if (shm_fdA == -1) {
               printf("Reading shared memory from shared memory A failed\n");
            //                fflush(stdout);
               exit(-1);
            }

            /* now map the shared memory segment in the address space of the process */
            void* ptrA;
            ptrA = mmap(NULL,SIZE, PROT_READ, MAP_SHARED, shm_fdA, 0);
            /* open the shared memory segment B */
            shm_fdB = shm_open(nameB, O_RDONLY, 0666);
            if (shm_fdB == -1) {
             printf("Reading shared memory from ChildB failed\n");
             exit(-1);
            }
            int size_b = 2*SIZE;
            /* now map the shared memory segment in the address space of the process */
            void* ptrB;

            ptrB = mmap(NULL,size_b, PROT_READ, MAP_SHARED, shm_fdB, 0);
            waitpid(pidB, &status, 0);
            waitpid(pidA, &status, 0);
            close(fd[0]);
            close(fd1[0]);
            if (ptrA == MAP_FAILED) {
               printf("Map A failed\n");
               exit(-1);
            }
            if (ptrB == MAP_FAILED) {
             printf("Map B failed\n");
             exit(-1);
            }
                
            /* now read from the shared memory regionA */
            //            fflush(stdout);
            printf("Reading shared memory from ChildA: \n -----------------o----------\n%s\n", (char*)ptrA);
            fflush(stdout);
            //             printf("SIZE: %d\n", SIZE);

            /* now read from the shared memory regionB */
            fprintf(stderr, "Reading shared memory from from Child B : \n -----------------o----------\n%s\n", (char*)ptrB);
    //        printf("Reading shared memory from from Child B : \n -----------------o----------\n  %s \n",ptr);
            fflush(stdout);

            signal(SIGQUIT, SIG_IGN);
            kill(-getpid(), SIGQUIT);
            /* remove the shared memory segment */
            if (shm_unlink(nameA) == -1) {
               printf("Error removing %s\n",nameA);
            exit(-1);
            }
            /* remove the shared memory segment */
            if (shm_unlink(nameB) == -1) {
             printf("Error removing %s\n",nameB);
             exit(-1);
            }
            exit(0);
       }
   // child B process
   else if(pidB == 0 ){
            close(fd1[1]);
            read(fd1[0], &SIZE, sizeof(SIZE)+1);
            //printf("Child B Received: %ld \n", SIZE);
            shm_fdB = shm_open(nameB, O_CREAT | O_RDWR, 0666);
            /* configure the size of the shared memory segment */
            ftruncate(shm_fdB,SIZE*2);
            /* now map the shared memory segment in the address space of the process */
            void* ptrB;
            ptrB = mmap(NULL,SIZE*2,PROT_WRITE, MAP_SHARED, shm_fdB, 0);
            //char* temp;
            buf_SIZE_B = 0;
            char buf;
            while(buf_SIZE_B < SIZE*2){
                read(fd1[0], &buf, sizeof(buf)+1);
                buf_SIZE_B+=sizeof(buf)*2;
                //temp = malloc(sizeof(line)*2);
                //string2hexString(line,temp);
                sprintf(ptrB, "%02X", buf);
                ptrB += sizeof(buf)*2;
                //free(temp);

            }
            
    //           if (ptrB == MAP_FAILED) {
    //            printf("MapB here failed\n");
    //            return -1;
    //           }

            
                exit(0);
        }
        
    }
    // child A process
    else {
            close(fd[1]);
            read(fd[0], &SIZE, sizeof(SIZE)+1);
            //printf("Child A received. %ld \n", SIZE);
            shm_fdA = shm_open(nameA, O_CREAT | O_RDWR, 0666);

            /* configure the size of the shared memory segment */
            ftruncate(shm_fdA,SIZE);
            void* ptrA;
            /* now map the shared memory segment in the address space of the process */
            ptrA = mmap(NULL,SIZE, PROT_WRITE, MAP_SHARED, shm_fdA, 0);
            buf_SIZE_A = 0;
            char buf;
            while(buf_SIZE_A < SIZE){
                read(fd[0], &buf, sizeof(char)+1);
                //printf("%c",c);
                //fflush(stdout);
                buf_SIZE += sizeof(buf);
                sprintf(ptrA, "%c", buf);
                ptrA += sizeof(buf);
            }
          
        //        if (ptrA == MAP_FAILED) {
        //            printf("MapA here failed\n");
        //            return -1;
        //        }
                exit(0);
        
    }
                                   
                                   
    
    
    return 0;
    
}






