//
//  chinese-whispers.c
//  COMP304-A1
//

#include "chinese-whispers.h"


int main(int __argc,char *__argv[])
{
    if(__argc <= 1)
    {
        printf("Arg Fail !\n");
        exit(1);
    }
    if(__argc > 2)
    {
        printf("Arg Fail !\n");
        exit(1);
    }
    if(strlen(__argv[1]) > 31)
    {
        printf("String too long.\n");
        exit(1);
    }
    size_t _n;
    pid_t _pid,_wpid;
    mqd_t _start_receive,_start_send;
    struct mq_attr _my,_mysend;
    mode_t _mode = S_IRUSR | S_IWUSR | S_IWGRP | S_IROTH | S_IWOTH;
    int status = 0;
    char data[strlen(__argv[1]) + 1];
    char name[60] = "";
    char final[60] = "";

    srand((unsigned)time(0));

    _my.mq_flags = 0; 
    _my.mq_maxmsg = 10; 
    _my.mq_msgsize = 32; 
    _my.mq_curmsgs = 0;

    sprintf(name, "%s", SERVER_QUEUE_NAME);
    strcat(name, "-1"); 
    _start_send = mq_open(name, O_CREAT | O_WRONLY, _mode, &(_my));
    //printf("%s\n",name);

    if(_start_send == -1){
        perror("OPEN ERROR");
    }

    strcpy(data, __argv[1]);
    int _rand = rand()%2;
    if(_rand != 0)
    {
        ++data[0];
    }
    if(mq_send(_start_send, data,strlen(data) + 1, 0) == -1) {
        perror("Sent failed!");
    }
    //printf("Parent stated ring by sending : %s\n", data); 


    for(size_t a = 1; a < strlen(__argv[1]);a++)
    {
        int __rand = rand()%2;
        _pid = fork();
        if(_pid == 0)
        {
            char temp[10];
            char _name[60];
            size_t _temp_a;
            _temp_a = a;
            //printf("Hello world :D\n");
            _PROCESS *p;
            p = (_PROCESS*)malloc(sizeof(_PROCESS));
            sprintf(p->name,"%s",SERVER_QUEUE_NAME);
            sprintf(temp,"-%zu",_temp_a);
            strcat(p->name,temp);
            //printf("%s Temp A is : %zu\n",p->name,_temp_a);
            p->server = mq_open(p->name,O_RDONLY,_mode,&(_my));
            if(p->server == -1)
            {
                perror("Open Fail");
                exit(1);
            }
            if(mq_receive(p->server,p->in_buf,sizeof(p->in_buf) + 10, 0) == -1)
            {
                perror("Error Receive");

            }
            //printf("Received whisper : %s\n",p->in_buf);
            _temp_a++;
            //printf("Whisper character : %c\n",p->in_buf[a]);
            if(__rand != 0)
            {
                 ++(p->in_buf[a]);
            }
            //(rand()%2 != 0 ? : printf("Not rand\n");
            if(a == (strlen(__argv[1]) - 1))
            {
                //printf("Final child sending to parent\n");
                _temp_a = 0;
                memset(temp,0,10);
                sprintf(_name,"%s",SERVER_QUEUE_NAME);
                sprintf(temp,"-%zu",_temp_a);
                strcat(_name,temp);
                p->client = mq_open(_name,O_CREAT | O_WRONLY,_mode,&(_my));
                if(mq_send(p->client,p->in_buf,strlen(p->in_buf)+1,0) == -1)
                {
                    perror("Send child error.");
                }
            }
            else
            {
                memset(temp,0,10);
                sprintf(_name,"%s",SERVER_QUEUE_NAME);
                sprintf(temp,"-%zu",_temp_a);
                strcat(_name,temp);
                p->client = mq_open(_name,O_CREAT | O_WRONLY,_mode,&(_my));
                if(mq_send(p->client,p->in_buf,strlen(p->in_buf)+1,0) == -1)
                {
                    perror("Send child error.");
                }
            }

            free(p);
            exit(0);
        }
        if(strlen(__argv[1]) > 15)
        {
             usleep(600);//need to sleep to avoid race condition
        }
        else
        {
             usleep(300);//need to sleep to avoid race condition
        }
       
    }
    
    while((_wpid = wait(&status)) > 0);
    //Do after every child is dead to complete the cycle
    //printf("All the child processes died ! Receiving from final child.\n");

    memset(name,0,60);
    sprintf(name,"%s",SERVER_QUEUE_NAME);
    strcat(name,"-0");
    //printf("%s\n",name);
    _start_receive = mq_open(name,O_RDONLY,_mode,&(_my));

    if(_start_receive == -1)
    {
        perror("Error");
        exit(1);
    }

    if(mq_receive(_start_receive,final,sizeof(final),0) == -1)
    {
        perror("Failed receiving final string from child.");
        exit(1);
    }

    printf("%s",final);

    for(size_t t = 0;t<strlen(__argv[1])+1;t++)
    {//Unlink
        char _unlink[16] = "";
        char _id[8] = "";
        sprintf(_unlink,"%s",SERVER_QUEUE_NAME);
        sprintf(_id,"-%zu",t);
        strcat(_unlink,_id);
        mq_unlink(_unlink);
    }
    
    return EXIT_SUCCESS;
}
