//
//  main.c
//  COMP304-A1
//
/*
 * shelly interface program

 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define MAX_LINE 160 /* 160 chars per line, per command, should be enough. */
#define TRUE 1
#define FALSE 0

typedef struct __struct_0
{
    char *key;
    char *value;//
//  main.c
//  COMP304-A1
//
/*
 * shelly interface program

 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define MAX_LINE 160 /* 160 chars per line, per command, should be enough. */
#define TRUE 1
#define FALSE 0

    typedef struct __struct_0
    {
        char *key;
        char *value;
        size_t _size;
        size_t _index;

    }KEY_VALUE;

    int parseCommand(char inputBuffer[], char *args[],int *background,ssize_t *size);

    char *findExecutable(char*, char*);

    KEY_VALUE* loadBookmarks(void);

    int main(void)
    {
        char inputBuffer[MAX_LINE];             /* buffer to hold the command entered */
        int background;                         /* equals 1 if a command is followed by '&' */
        char *args[MAX_LINE/2 + 1];             /* command line (of 80) has max of 40 arguments */
        pid_t child;                            /* process id of the child process */
        int status;                             /* result from execv system call*/
        int shouldrun = 1;
        char readBuf[512 + 1] = "";
        char *fullPath;
        ssize_t argSize;
        int concat_trunc;
        ssize_t t;
        KEY_VALUE* _bookmark = NULL;


        //int i, upper;

        while (shouldrun){                    /* Program terminates normally inside setup */
            background = 0;
            argSize = 0;
            concat_trunc = 0;

            //printf("%d\n",getpid());
            shouldrun = parseCommand(inputBuffer,args,&background,&argSize);       /* get next command */

            if (strncmp(inputBuffer, "exit", 4) == 0)
            {
                shouldrun = 0;     /* Exiting from myshell*/
                if(_bookmark != NULL)
                {
                    free(_bookmark);
                }
            }

            if (shouldrun)
            {
                /*
                 After reading user input, the steps are
                 (1) Fork a child process using fork()
                 (2) the child process will invoke execv()
                 (3) if command included &, parent will invoke wait()
                 */
                int __pipe[2];
                ssize_t __length;
                fullPath = findExecutable(getenv("PATH"),args[0]);
                if(strcmp(args[0],"script") == 0)
                {
                    FILE * fd;
                    fd = fopen(args[1],"w");
                    while(1)
                    {
                        memset(inputBuffer,0,MAX_LINE);
                        do {
                            printf("shelly>");
                            fflush(stdout);
                            __length = read(STDIN_FILENO,inputBuffer,MAX_LINE);
                        }
                        while (inputBuffer[0] == '\n'); /* swallow newline characters */
                        if(strncmp(inputBuffer,"exit",4) == 0)
                        {
                            break;
                        }
                        fprintf(fd,"%s",inputBuffer);
                    }
                    fclose(fd);
                    continue;
                }
                if(strcmp(args[0],"bookmark") == 0)
                {
                    if(strcmp(args[1],"-r") == 0)
                    {
                        char *_path;
                        FILE *fq;
                        _path = malloc(96*sizeof(char));
                        strcpy(_path, getenv("HOME"));
                        strcat(_path,"/.mybookmark");
                        strcat(_path,"\0");
                        fq = fopen(_path,"w");
                        fclose(fq);
                        free(_path);
                        if(_bookmark != NULL)
                        {
                            free(_bookmark);
                            _bookmark = NULL;
                        }
                        continue;
                    }
                    if(argSize < 3)
                    {
                        printf("Bookmark wrong usage !\n");
                        continue;
                    }
                    if((args[2][0] != '\"') )
                    {
                        printf("Bookmark wrong usage !\n");
                        continue;
                    }
                    _bookmark = loadBookmarks();
                    int _w = 0;
                    for(size_t _t = 0;_t<_bookmark->_index;_t++)
                    {
                        if(strcmp(_bookmark[_t].key, args[1]) == 0)
                        {
                            printf("Key already exists ! Cannot add !\n");
                            _w = TRUE;
                            break;
                        }
                    }
                    if(_w == TRUE)
                    {
                        continue;
                    }
                    _bookmark[_bookmark->_index].key = args[1];
                    _bookmark[_bookmark->_index].value = args[2];
                    if(args[2][strlen(args[2]) - 1 ] != '\"')
                    {
                        for(size_t _q = 3 ; _q < argSize;_q++)
                        {
                            //printf("Will enter here\n");
                            if(args[_q][strlen(args[_q]) - 1 ] == '\"')
                            {
                                char *argMerge = (char*)malloc(strlen(args[_q-1]) + strlen(args[_q]) + 2 + strlen(" "));
                                strcpy(argMerge,args[_q-1]);
                                strcat(argMerge,"\0");
                                strcat(argMerge," ");
                                strcat(argMerge,args[_q]);
                                _bookmark[_bookmark->_index].value = argMerge;
                                //printf("Bookmark key : %s Value : %s\n",_bookmark[_bookmark->_index].key,_bookmark[_bookmark->_index].value);
                                break;
                            }
                            else
                            {
                                //printf("Else e girmemesi lazim\n");
                                char *argMerge = (char*)malloc(strlen(args[_q-1]) + strlen(args[_q]) + 2 + strlen(" "));
                                strcpy(argMerge,args[_q-1]);
                                strcat(argMerge,"\0");
                                strcat(argMerge," ");
                                strcat(argMerge,args[_q]);
                                args[_q] = argMerge;
                            }
                        }
                    }
                    _bookmark->_index++;
                    if(_bookmark->_index >= _bookmark->_size)
                    {
                        _bookmark->_size = _bookmark->_size * 2;
                        _bookmark = (KEY_VALUE*)realloc(_bookmark,_bookmark->_size);
                    }

                    FILE *fa;
                    char file_path[65] = "";
                    strcat(file_path,getenv("HOME"));
                    strcat(file_path,"/.mybookmark");
                    fa = fopen(file_path,"w");
                    for(size_t a = 0;a<_bookmark->_index;a++)
                    {
                        //printf("Key : %s Value : %s\n",_bookmark[a].key,_bookmark[a].value);
                        fprintf(fa,"%s %s\n",_bookmark[a].key,_bookmark[a].value);
                    }
                    fclose(fa);
                    //free(argMerge);
                    for(size_t qwe = 0;qwe < argSize;qwe++)
                    {
                        args[qwe] = NULL;
                    }
                    continue;
                }
                if(strcmp(args[0],"cd") == 0)
                {
                    if(chdir(args[1]) != 0)
                    {
                        perror(args[1]);
                    }
                    continue;
                }
                if(strcmp(args[0],"wforecast") == 0)
                {
                    FILE *cron_file,*prev_cron;
                    int s;
                    pid_t _pid;
                    char *_args[3];
                    char *cron_command;
                    char cronTAB[64] = "";
                    char readB[64];
                    cron_command = (char*)malloc(sizeof(char)*128);
                    strcpy(cron_command,"\0");
                    strcat(cron_command,"0 9 * * *");
                    strcat(cron_command," curl wttr.in/Istanbul.png > ");
                    strcat(cron_command,getenv("HOME"));
                    strcat(cron_command,"/Desktop/");// for now png will be created in desktop TODO: Change this if necessary
                    strcat(cron_command,args[1]);
                    strcat(cronTAB,findExecutable(getenv("PATH"), "crontab"));
                    strcat(cronTAB," -l");
                    cron_file = fopen(".tempcron","w");
                    prev_cron = popen(cronTAB, "r"); // if there is no crontab for a user then this will cause error ? idk how to fix :(
                    while(fgets(readB,sizeof(readB),prev_cron) != NULL)
                    {
                        fprintf(cron_file,"%s",readB);
                    }
                    fprintf(cron_file,"%s\n",cron_command);
                    _args[0] = findExecutable(getenv("PATH"), "crontab");
                    _args[1] = ".tempcron";
                    _args[2] = NULL;
                    fclose(cron_file);
                    fclose(prev_cron);
                    _pid = fork();
                    if(_pid == 0)
                    {
                        s = execv(_args[0],_args);
                    }
                    //printf("0 9 * * * ");
                    continue;
                }
                if(strcmp(args[0],"whispers") == 0)
                {
                    if(args[1] == NULL)
                    {
                        printf("Argument Error !\n");
                        continue;
                    }
                    if(args[2] != NULL)
                    {
                        printf("Too many arguments\n");
                        continue;
                    }
                    char *__exec;
                    char *_arg1;
                    char whisper_buf[256];
                    FILE *fp;
                    //printf("args 1 : %s\n",args[1]);
                    _arg1 = (char*)malloc(strlen(args[1]) + 1);
                    //strcpy(_arg1,"\0");
                    strncpy(_arg1,args[1]+1,(strlen(args[1])-2));
                    strcat(_arg1,"\0");
                    // printf("%s\n",_arg1);
                    __exec = (char*)malloc(((strlen(_arg1)+1) + strlen(args[0])+1));
                    strcpy(__exec,"\0");
                    strcat(__exec,"./");
                    strcat(__exec,args[0]);
                    strcat(__exec," ");
                    strcat(__exec,_arg1);
                    fp = popen(__exec,"r");
                    if(fp == NULL)
                    {
                        printf("Failed to execute command : %s\n",__exec);
                        continue;
                    }
                    while(fgets(whisper_buf,sizeof(whisper_buf) - 1,fp) != NULL)
                    {
                        printf("%s\n",whisper_buf);
                    }
                    continue;
                }
                if(strcmp(args[0],"currency") == 0)
                {//our command
                    if((args[1] == NULL) || (args[2]==NULL))
                    {
                        printf("Argument Error\n");
                        continue;
                    }
                    if(args[3] != NULL)
                    {
                        printf("Too many arguments!\n");
                        continue;
                    }
                    FILE *fg;
                    char __buf[256];
                    char *site;
                    site = (char*)malloc(256);
                    strcpy(site ,"\0");
                    strcat(site, "curl -sS \"http://www.xe.com/currencyconverter/convert/?Amount=1&From=");
                    strcat(site, args[1]);
                    strcat(site,"&To=");
                    strcat(site,args[2]);
                    strcat(site, "\" | grep uccResultAmount");

                    fg = popen(site,"r");
                    while(fgets(__buf,sizeof(__buf),fg) != NULL)
                    {
                        char *pointer;
                        if((pointer = strstr(__buf, "uccResultAmount")) != NULL)
                        {
                            //printf("%s\n", pointer);
                            char *pointer2;
                            if((pointer2 = strstr(pointer, ">")) != NULL)
                            {
                                //printf("%s\n", pointer2);
                                char *lptr;
                                lptr = (char*)malloc(16);
                                strncpy(lptr, pointer2 + 1, 7);
                                //printf("%s\n", lptr);
                                FILE *fe;
                                char *cmd;
                                char *currencies;
                                currencies = (char*)malloc(64);
                                strcpy(currencies, "\0");
                                strcat(currencies, "notify-send \"Currency: ");
                                strcat(currencies, args[1]);
                                strcat(currencies, " - ");
                                strcat(currencies, args[2]);
                                cmd = (char*)malloc(strlen(lptr) + strlen(currencies) + 16);
                                strcpy(cmd, "\0");
                                strcat(cmd, currencies);
                                strcat(cmd, " ");
                                strcat(cmd, lptr);
                                strcat(cmd,"\"");
                                fe = popen(cmd,"r");
                                fclose(fe);
                                free(currencies);
                                free(cmd);
                                free(lptr);
                                break;
                            }
                        }
                        //printf("%s",__buf);
                    }
                    free(site);
                    continue;
                }
                if(fullPath == NULL)
                {
                    int _break = 1;
                    char *command_extract;
                    if(_bookmark == NULL)
                    {
                        _bookmark = loadBookmarks();
                    }
                    command_extract = (char*)malloc(64);
                    for(size_t a = 0;a < _bookmark->_index;a++)
                    {
                        if(strcmp(args[0],_bookmark[a].key) == 0)
                        {
                            char *tkn,*dup;
                            size_t __s = 0;
                            strncpy(command_extract,_bookmark[a].value + 1,strlen(_bookmark[a].value));
                            command_extract[strcspn(command_extract,"\"")] = 0;
                            dup = strdup(command_extract);

                            while((tkn = strsep(&dup," ")) != NULL)
                            {
                                //printf("Token : %s\n",tkn);
                                if(__s == 0)
                                {
                                    args[__s] = findExecutable(getenv("PATH"), tkn);
                                }
                                else
                                {

                                    //tkn[strcspn(tkn, "\n")] = 0;
                                    args[__s] = tkn;
                                }
                                __s++;
                            }
                            args[__s] = NULL;
                            _break = 0;
                        }
                    }
                    if(_break == 1)
                    {
                        printf("Could not locate file/executable : %s\n",args[0]);
                        continue;
                    }
                }
                for(t = 0;t < argSize ; t++)
                {
                    //printf("ARGUMENT : %s\n",args[t]);
                    if(strstr(args[t],">"))
                    {
                        if(strstr(args[t],">>"))
                        {
                            //printf("Contains >>\n");
                            concat_trunc = -1;
                            break;
                        }
                        //printf("Contains >\n");
                        concat_trunc = 1;
                        break;
                    }
                }
                if(background)
                {
                    //printf("Entering Background.\n");
                    child = fork();
                    if(child == -1)
                    {
                        perror("Error forking child process.\n");
                    }
                    else if(child == 0)//Child
                    {

                        printf("Process id : %d\n",getpid());
                        //printf("%s\n",fullPath);
                        if(fullPath != NULL)
                        {
                            args[0] = fullPath;
                        }
                        status = execv(args[0],args);
                        exit(0);
                    }
                    else //Parent
                    {
                        //printf("Parent Process id : %d\n",getpid());
                        wait(&status);
                        //printf("DONE!\n");
                    }
                }
                else
                {
                    pipe(__pipe);
                    //printf("Entered Else.\n");
                    child = fork();
                    if(child == -1)
                    {
                        perror("Error forking child process.\n");
                    }
                    else if(child == 0)//Child
                    {
                        close(__pipe[0]);
                        dup2(__pipe[1],STDOUT_FILENO);
                        close(__pipe[1]);
                        if(fullPath != NULL)
                        {
                            args[0] = fullPath;
                        }
                        //printf("Args[0] : %s\nArgs[1] : %s\nArgs[2] : %s\n",args[0],args[1],args[2]);
                        if(concat_trunc != 0)
                        {
                            //TODO: implement > and >> DONE
                            args[t] = NULL;
                        }
                        status = execv(args[0],args);
                        printf("Child Failed.\n");
                        exit(1);
                        //printf("Status : %d\n",status);
                    }
                    else//Parent
                    {
                        char *_stdout;
                        FILE *fp = NULL;
                        size_t _size = 513;
                        close(__pipe[1]);
                        _stdout = (char*)malloc(_size*sizeof(char));
                        memset(_stdout,0, _size);
                        //printf("Parent Reading.\n");
                        if(concat_trunc > 0)
                        {
                            //printf("Here \n");
                            fp = fopen(args[t+1],"w");
                        }
                        else if(concat_trunc < 0)
                        {
                            //printf("HERE ?\n");
                            fp = fopen(args[t+1],"a+");

                        }
                        while (read(__pipe[0], readBuf, sizeof(readBuf)) != 0)
                        {
                            if(_size == 513)
                            {
                                strcpy(_stdout, readBuf);
                                strcat(_stdout,"\0");
                            }
                            else
                            {
                                strcat(_stdout,readBuf);
                            }
                            //strcat(_stdout,"\0");
                            memset(readBuf,0,513);
                            if(concat_trunc == 0)
                            {
                                //print only
                                printf("%s",_stdout);
                            }
                            else if(concat_trunc > 0)
                            {
                                //Truncate
//                            fp = fopen(args[t+1],"w");
                                fprintf(fp,"%s",_stdout);
                            }
                            else
                            {
                                //printf("Concat op \n");
                                //Concat
//                            fp = fopen(args[t+1],"a+");
                                fprintf(fp,"%s",_stdout);
                            }
//                        _size *= 2;
//                        _stdout = (char*)realloc(_stdout, _size);
//                        printf("Did realloc Size : %zu\n",_size);
                        }
                        if(fp != NULL)
                        {
                            fclose(fp);
                        }
                        close(__pipe[0]);
                    }
                }
            }

        }
        return EXIT_SUCCESS;
    }

/**
 * The parseCommand function below will not return any value, but it will just: read
 * in the next command line; separate it into distinct arguments (using blanks as
 * delimiters), and set the args array entries to point to the beginning of what
 * will become null-terminated, C-style strings.
 */

    int parseCommand(char inputBuffer[], char *args[],int *background,ssize_t *size)
    {
        ssize_t  length,        /* # of characters in the command line */
        i,        /* loop index for accessing inputBuffer array */
        start,        /* index where beginning of next command parameter is */
        ct;        /* index of where to place the next parameter into args[] */
        //command_number;    /* index of requested command number */

        ct = 0;

        /* read what the user enters on the command line */
        do {
            //memset(inputBuffer, 0, MAX_LINE);
            printf("shelly>");
            fflush(stdout);
            length = read(STDIN_FILENO,inputBuffer,MAX_LINE);
        }
        while (inputBuffer[0] == '\n'); /* swallow newline characters */

        /**
         *  0 is the system predefined file descriptor for stdin (standard input),
         *  which is the user's screen in this case. inputBuffer by itself is the
         *  same as &inputBuffer[0], i.e. the starting address of where to store
         *  the command that is read, and length holds the number of characters
         *  read in. inputBuffer is not a null terminated C-string.
         */
        start = -1;
        if (length == 0)
            exit(0);            /* ^d was entered, end of user command stream */

        /**
         * the <control><d> signal interrupted the read system call
         * if the process is in the read() system call, read returns -1
         * However, if this occurs, errno is set to EINTR. We can check this  value
         * and disregard the -1 value
         */

        if ( (length < 0) && (errno != EINTR) ) {
            perror("error reading the command");
            exit(-1);           /* terminate with error code of -1 */
        }

        /**
         * Parse the contents of inputBuffer
         */

        for (i=0;i<length;i++) {
            /* examine every character in the inputBuffer */
            switch (inputBuffer[i]){
                case ' ':
                case '\t' :               /* argument separators */
                    if(start != -1){
                        args[ct] = &inputBuffer[start];    /* set up pointer */
                        ct++;
                    }
                    inputBuffer[i] = '\0'; /* add a null char; make a C string */
                    start = -1;
                    break;

                case '\n':                 /* should be the final char examined */
                    if (start != -1){
                        args[ct] = &inputBuffer[start];
                        ct++;
                    }
                    inputBuffer[i] = '\0';
                    args[ct] = NULL; /* no more arguments to this command */
                    break;

                default :             /* some other character */
                    if (start == -1)
                        start = i;
                    if (inputBuffer[i] == '&') {
                        *background  = 1;
                        inputBuffer[i-1] = '\0';
                    }
            } /* end of switch */
        }    /* end of for */

        /**
         * If we get &, don't enter it in the args array
         */

        if (*background)
            args[--ct] = NULL;

        *size = ct;

        args[ct] = NULL; /* just in case the input line was > 80 */

        return 1;

    } /* end of parseCommand routine */

    char *findExecutable(char *path,char *exe)
    {
        char *result,*seperate,*dup;
        result = (char*)malloc(128*sizeof(char));
        dup = strdup(path);
        //printf("Exec : %s\n",exe);
        while((seperate = strsep(&dup,":")) != NULL )
        {
            if(strstr(seperate,"Applications"))
            {
                continue;
            }
            strcat(result,seperate);
            strcat(result,"/");
            strcat(result,exe);
            if(access( result, F_OK ) != -1 )
            {
                //printf("EXEC : %s\n",exe);
                return result;
            }
            memset(result,0,128);
        }
        return NULL;
    }

    KEY_VALUE* loadBookmarks(void)
    {
        FILE *ff;
        char file_path[65] = "";
        char readBuffer[256];
        char *token,*duplicate;
        KEY_VALUE *key_value = NULL;
        size_t _i = 0;
        int t = 0;
        key_value = (KEY_VALUE*)malloc(sizeof(KEY_VALUE)*16);
        key_value->_size = 16;
        key_value->_index = 0;
        strcat(file_path,getenv("HOME"));
        strcat(file_path,"/.mybookmark");
        //printf("File path : %s\n",file_path);
        ff = fopen(file_path,"r");
        if(ff)
        {
            while(fgets(readBuffer, sizeof(readBuffer), ff))
            {
                char *argsBuffer;
                argsBuffer = (char*)malloc(256*sizeof(char));
                t = 0;
                readBuffer[strcspn(readBuffer, "\n")] = 0;
                //printf("%s\n",readBuffer);
                duplicate = strdup(readBuffer);
                if(strlen(readBuffer) != 0)
                {
                    while((token = strsep(&duplicate," ")) != NULL)
                    {
                        //printf("Tokenizer girdi Token : %s\n",token);
                        if(t == 0)
                        {
                            key_value[_i].key = token;
                            t = 1;
                        }
                        else
                        {
                            if(t == 1)
                            {
                                strcpy(argsBuffer,token);
                            }
                            else
                            {
                                strcat(argsBuffer," ");
                                strcat(argsBuffer,token);
                            }
                            t++;
                            key_value[_i].value = argsBuffer;
                        }
                    }
                    //printf("Key : %s | Value : %s | i : %zu\n",key_value[_i].key,key_value[_i].value,_i);
                    _i++;

                    if(_i >= key_value->_size)
                    {
                        key_value->_size = key_value->_size * 2;
                        key_value = (KEY_VALUE*)realloc(key_value,key_value->_size);
                    }
                }
            }

            key_value->_index = _i;
            //free(argsBuffer);
            fclose(ff);
        }
        return key_value;
    }


    size_t _size;
    size_t _index;
    
}KEY_VALUE;

int parseCommand(char inputBuffer[], char *args[],int *background,ssize_t *size);

char *findExecutable(char*, char*);

KEY_VALUE* loadBookmarks(void);

int main(void)
{
    char inputBuffer[MAX_LINE];             /* buffer to hold the command entered */
    int background;                         /* equals 1 if a command is followed by '&' */
    char *args[MAX_LINE/2 + 1];             /* command line (of 80) has max of 40 arguments */
    pid_t child;                            /* process id of the child process */
    int status;                             /* result from execv system call*/
    int shouldrun = 1;
    char readBuf[512 + 1] = "";
    char *fullPath;
    ssize_t argSize;
    int concat_trunc;
    ssize_t t;
    KEY_VALUE* _bookmark = NULL;

    
    //int i, upper;
    
    while (shouldrun){                    /* Program terminates normally inside setup */
        background = 0;
        argSize = 0;
        concat_trunc = 0;
        
        //printf("%d\n",getpid());
        shouldrun = parseCommand(inputBuffer,args,&background,&argSize);       /* get next command */
        
        if (strncmp(inputBuffer, "exit", 4) == 0)
        {
            shouldrun = 0;     /* Exiting from myshell*/
            if(_bookmark != NULL)
            {
                free(_bookmark);
            }
        }

        if (shouldrun)
        {
            /*
             After reading user input, the steps are
             (1) Fork a child process using fork()
             (2) the child process will invoke execv()
             (3) if command included &, parent will invoke wait()
             */
            int __pipe[2];
            ssize_t __length;
            fullPath = findExecutable(getenv("PATH"),args[0]);
            if(strcmp(args[0],"script") == 0)
            {
                FILE * fd;
                fd = fopen(args[1],"w");
                while(1)
                {
                    memset(inputBuffer,0,MAX_LINE);
                    do {
                        printf("shelly>");
                        fflush(stdout);
                        __length = read(STDIN_FILENO,inputBuffer,MAX_LINE);
                    }
                    while (inputBuffer[0] == '\n'); /* swallow newline characters */
                    if(strncmp(inputBuffer,"exit",4) == 0)
                    {
                        break;
                    }
                    fprintf(fd,"%s",inputBuffer);
                }
                fclose(fd);
                continue;
            }
            if(strcmp(args[0],"bookmark") == 0)
            {
                if(strcmp(args[1],"-r") == 0)
                {
                    char *_path;
                    FILE *fq;
                    _path = malloc(96*sizeof(char));
                    strcpy(_path, getenv("HOME"));
                    strcat(_path,"/.mybookmark");
                    strcat(_path,"\0");
                    fq = fopen(_path,"w");
                    fclose(fq);
                    free(_path);
                    if(_bookmark != NULL)
                    {
                        free(_bookmark);
                        _bookmark = NULL;
                    }
                    continue;
                }
                if(argSize < 3)
                {
                    printf("Bookmark wrong usage !\n");
                    continue;
                }
                if((args[2][0] != '\"') )
                {
                    printf("Bookmark wrong usage !\n");
                    continue;
                }
                _bookmark = loadBookmarks();
                int _w = 0;
                for(size_t _t = 0;_t<_bookmark->_index;_t++)
                {
                    if(strcmp(_bookmark[_t].key, args[1]) == 0)
                    {
                        printf("Key already exists ! Cannot add !\n");
                        _w = TRUE;
                        break;
                    }
                }
                if(_w == TRUE)
                {
                    continue;
                }
                _bookmark[_bookmark->_index].key = args[1];
                _bookmark[_bookmark->_index].value = args[2];
                if(args[2][strlen(args[2]) - 1 ] != '\"')
                {
                    for(size_t _q = 3 ; _q < argSize;_q++)
                    {
                        //printf("Will enter here\n");
                        if(args[_q][strlen(args[_q]) - 1 ] == '\"')
                        {
                            char *argMerge = (char*)malloc(strlen(args[_q-1]) + strlen(args[_q]) + 2 + strlen(" "));
                            strcpy(argMerge,args[_q-1]);
                            strcat(argMerge,"\0");
                            strcat(argMerge," ");
                            strcat(argMerge,args[_q]);
                            _bookmark[_bookmark->_index].value = argMerge;
                            //printf("Bookmark key : %s Value : %s\n",_bookmark[_bookmark->_index].key,_bookmark[_bookmark->_index].value);
                            break;
                        }
                        else
                        {
                            //printf("Else e girmemesi lazim\n");
                            char *argMerge = (char*)malloc(strlen(args[_q-1]) + strlen(args[_q]) + 2 + strlen(" "));
                            strcpy(argMerge,args[_q-1]);
                            strcat(argMerge,"\0");
                            strcat(argMerge," ");
                            strcat(argMerge,args[_q]);
                            args[_q] = argMerge;
                        }
                    }
                }
                _bookmark->_index++;
                if(_bookmark->_index >= _bookmark->_size)
                {
                    _bookmark->_size = _bookmark->_size * 2;
                    _bookmark = (KEY_VALUE*)realloc(_bookmark,_bookmark->_size);
                }
                
                FILE *fa;
                char file_path[65] = "";
                strcat(file_path,getenv("HOME"));
                strcat(file_path,"/.mybookmark");
                fa = fopen(file_path,"w");
                for(size_t a = 0;a<_bookmark->_index;a++)
                {
                    //printf("Key : %s Value : %s\n",_bookmark[a].key,_bookmark[a].value);
                    fprintf(fa,"%s %s\n",_bookmark[a].key,_bookmark[a].value);
                }
                fclose(fa);
                //free(argMerge);
                for(size_t qwe = 0;qwe < argSize;qwe++)
                {
                    args[qwe] = NULL;
                }
                continue;
            }
            if(strcmp(args[0],"cd") == 0)
            {
                if(chdir(args[1]) != 0)
                {
                    perror(args[1]);
                }
                continue;
            }
            if(strcmp(args[0],"wforecast") == 0)
            {
                FILE *cron_file,*prev_cron;
                int s;
                pid_t _pid;
                char *_args[3];
                char *cron_command;
                char cronTAB[64] = "";
                char readB[64];
                cron_command = (char*)malloc(sizeof(char)*128);
                strcpy(cron_command,"\0");
                strcat(cron_command,"0 9 * * *");
                strcat(cron_command," curl wttr.in/Istanbul.png > ");
                strcat(cron_command,getenv("HOME"));
                strcat(cron_command,"/Desktop/");// for now png will be created in desktop TODO: Change this if necessary
                strcat(cron_command,args[1]);
                strcat(cronTAB,findExecutable(getenv("PATH"), "crontab"));
                strcat(cronTAB," -l");
                cron_file = fopen(".tempcron","w");
                prev_cron = popen(cronTAB, "r"); // if there is no crontab for a user then this will cause error ? idk how to fix :(
                while(fgets(readB,sizeof(readB),prev_cron) != NULL)
                {
                    fprintf(cron_file,"%s",readB);
                }
                fprintf(cron_file,"%s\n",cron_command);
                _args[0] = findExecutable(getenv("PATH"), "crontab");
                _args[1] = ".tempcron";
                _args[2] = NULL;
                fclose(cron_file);
                fclose(prev_cron);
                _pid = fork();
                if(_pid == 0)
                {
                    s = execv(_args[0],_args);
                }
                //printf("0 9 * * * ");
                continue;
            }
            if(strcmp(args[0],"whispers") == 0)
            {
                if(args[1] == NULL)
                {
                    printf("Argument Error !\n");
                    continue;
                }
                if(args[2] != NULL)
                {
                    printf("Too many arguments\n");
                    continue;
                }
                char *__exec;
                char *_arg1;
                char whisper_buf[256];
                FILE *fp;
                //printf("args 1 : %s\n",args[1]);
                _arg1 = (char*)malloc(strlen(args[1]) + 1);
                //strcpy(_arg1,"\0");
                strncpy(_arg1,args[1]+1,(strlen(args[1])-2));
                strcat(_arg1,"\0");
               // printf("%s\n",_arg1);
                __exec = (char*)malloc(((strlen(_arg1)+1) + strlen(args[0])+1));
                strcpy(__exec,"\0");
                strcat(__exec,"./");
                strcat(__exec,args[0]);
                strcat(__exec," ");
                strcat(__exec,_arg1);
                fp = popen(__exec,"r");
                if(fp == NULL)
                {
                    printf("Failed to execute command : %s\n",__exec);
                    continue;
                }
                while(fgets(whisper_buf,sizeof(whisper_buf) - 1,fp) != NULL)
                {
                    printf("%s\n",whisper_buf);
                }
                continue;
            }
            if(strcmp(args[0],"currency") == 0)
            {//our command
                if((args[1] == NULL) || (args[2]==NULL))
                {
                    printf("Argument Error\n");
                    continue;
                }
                if(args[3] != NULL)
                {
                    printf("Too many arguments!\n");
                    continue;
                }
                FILE *fg;
                char __buf[256];
                char *site;
                site = (char*)malloc(256);
                strcpy(site ,"\0");
                strcat(site, "curl -sS \"http://www.xe.com/currencyconverter/convert/?Amount=1&From=");
                strcat(site, args[1]);
                strcat(site,"&To=");
                strcat(site,args[2]);
                strcat(site, "\" | grep uccResultAmount");

                fg = popen(site,"r");
                while(fgets(__buf,sizeof(__buf),fg) != NULL)
                {
                    char *pointer;
                    if((pointer = strstr(__buf, "uccResultAmount")) != NULL) 
                    {
                        //printf("%s\n", pointer);
                        char *pointer2;
                        if((pointer2 = strstr(pointer, ">")) != NULL)
                        {
                            //printf("%s\n", pointer2);
                            char *lptr;
                            lptr = (char*)malloc(16);
                            strncpy(lptr, pointer2 + 1, 7);
                            //printf("%s\n", lptr);
                            FILE *fe;
                            char *cmd;
                            char *currencies;
                            currencies = (char*)malloc(64);
                            strcpy(currencies, "\0");
                            strcat(currencies, "notify-send \"Currency: ");
                            strcat(currencies, args[1]);
                            strcat(currencies, " - ");
                            strcat(currencies, args[2]);
                            cmd = (char*)malloc(strlen(lptr) + strlen(currencies) + 16); 
                            strcpy(cmd, "\0");
                            strcat(cmd, currencies);
                            strcat(cmd, " ");
                            strcat(cmd, lptr);
                            strcat(cmd,"\"");
                            fe = popen(cmd,"r");
                            fclose(fe);
                            free(currencies);
                            free(cmd);
                            free(lptr);
                            break;
                        }
                    }
                    //printf("%s",__buf); 
                }
                free(site);
                continue;
            }
            if(fullPath == NULL)
            {
                int _break = 1;
                char *command_extract;
                if(_bookmark == NULL)
                {
                    _bookmark = loadBookmarks();
                }
                command_extract = (char*)malloc(64);
                for(size_t a = 0;a < _bookmark->_index;a++)
                {
                    if(strcmp(args[0],_bookmark[a].key) == 0)
                    {
                        char *tkn,*dup;
                        size_t __s = 0;
                        strncpy(command_extract,_bookmark[a].value + 1,strlen(_bookmark[a].value));
                        command_extract[strcspn(command_extract,"\"")] = 0;
                        dup = strdup(command_extract);

                        while((tkn = strsep(&dup," ")) != NULL)
                        {
                            //printf("Token : %s\n",tkn);
                            if(__s == 0)
                            {
                                args[__s] = findExecutable(getenv("PATH"), tkn);
                            }
                            else
                            {

                                //tkn[strcspn(tkn, "\n")] = 0;
                                args[__s] = tkn;
                            }
                            __s++;
                        }
                        args[__s] = NULL;
                        _break = 0;
                    }
                }
                if(_break == 1)
                {
                    printf("Could not locate file/executable : %s\n",args[0]);
                    continue;
                }
            }
            for(t = 0;t < argSize ; t++)
            {
                //printf("ARGUMENT : %s\n",args[t]);
                if(strstr(args[t],">"))
                {
                    if(strstr(args[t],">>"))
                    {
                        //printf("Contains >>\n");
                        concat_trunc = -1;
                        break;
                    }
                    //printf("Contains >\n");
                    concat_trunc = 1;
                    break;
                }
            }
            if(background)
            {
                //printf("Entering Background.\n");
                child = fork();
                if(child == -1)
                {
                    perror("Error forking child process.\n");
                }
                else if(child == 0)//Child
                {
                    
                    printf("Process id : %d\n",getpid());
                    //printf("%s\n",fullPath);
                    if(fullPath != NULL)
                    {
                        args[0] = fullPath;
                    }
                    status = execv(args[0],args);
                    exit(0);
                }
                else //Parent
                {
                    //printf("Parent Process id : %d\n",getpid());
                    wait(&status);
                    //printf("DONE!\n");      
                }
            }
            else
            {
                pipe(__pipe);
                //printf("Entered Else.\n");
                child = fork();
                if(child == -1)
                {
                    perror("Error forking child process.\n");
                }
                else if(child == 0)//Child
                {
                    close(__pipe[0]);
                    dup2(__pipe[1],STDOUT_FILENO);
                    close(__pipe[1]);
                    if(fullPath != NULL)
                    {
                        args[0] = fullPath;
                    }
                    //printf("Args[0] : %s\nArgs[1] : %s\nArgs[2] : %s\n",args[0],args[1],args[2]);
                    if(concat_trunc != 0)
                    {
                        //TODO: implement > and >> DONE
                        args[t] = NULL;
                    }
                    status = execv(args[0],args);
                    printf("Child Failed.\n");
                    exit(1);
                    //printf("Status : %d\n",status);
                }
                else//Parent
                {
                    char *_stdout;
                    FILE *fp = NULL;
                    size_t _size = 513;
                    close(__pipe[1]);
                    _stdout = (char*)malloc(_size*sizeof(char));
                    memset(_stdout,0, _size);
                    //printf("Parent Reading.\n");
                    if(concat_trunc > 0)
                    {
                        //printf("Here \n");
                        fp = fopen(args[t+1],"w");
                    }
                    else if(concat_trunc < 0)
                    {
                        //printf("HERE ?\n");
                        fp = fopen(args[t+1],"a+");
                        
                    }
                    while (read(__pipe[0], readBuf, sizeof(readBuf)) != 0)
                    {
                        if(_size == 513)
                        {
                            strcpy(_stdout, readBuf);
                            strcat(_stdout,"\0");
                        }
                        else
                        {
                            strcat(_stdout,readBuf);
                        }
                        //strcat(_stdout,"\0");
                        memset(readBuf,0,513);
                        if(concat_trunc == 0)
                        {
                            //print only
                            printf("%s",_stdout);
                        }
                        else if(concat_trunc > 0)
                        {
                            //Truncate
//                            fp = fopen(args[t+1],"w");
                            fprintf(fp,"%s",_stdout);
                        }
                        else
                        {
                            //printf("Concat op \n");
                            //Concat
//                            fp = fopen(args[t+1],"a+");
                            fprintf(fp,"%s",_stdout);
                        }
//                        _size *= 2;
//                        _stdout = (char*)realloc(_stdout, _size);
//                        printf("Did realloc Size : %zu\n",_size);
                    }
                    if(fp != NULL)
                    {
                         fclose(fp);
                    }
                    close(__pipe[0]);
                }
            }
        }
        
    }
    return EXIT_SUCCESS;
}

/**
 * The parseCommand function below will not return any value, but it will just: read
 * in the next command line; separate it into distinct arguments (using blanks as
 * delimiters), and set the args array entries to point to the beginning of what
 * will become null-terminated, C-style strings.
 */

int parseCommand(char inputBuffer[], char *args[],int *background,ssize_t *size)
{
    ssize_t  length,        /* # of characters in the command line */
    i,        /* loop index for accessing inputBuffer array */
    start,        /* index where beginning of next command parameter is */
    ct;        /* index of where to place the next parameter into args[] */
    //command_number;    /* index of requested command number */
    
    ct = 0;
    
    /* read what the user enters on the command line */
    do {
        //memset(inputBuffer, 0, MAX_LINE);
        printf("shelly>");
        fflush(stdout);
        length = read(STDIN_FILENO,inputBuffer,MAX_LINE);
    }
    while (inputBuffer[0] == '\n'); /* swallow newline characters */
    
    /**
     *  0 is the system predefined file descriptor for stdin (standard input),
     *  which is the user's screen in this case. inputBuffer by itself is the
     *  same as &inputBuffer[0], i.e. the starting address of where to store
     *  the command that is read, and length holds the number of characters
     *  read in. inputBuffer is not a null terminated C-string.
     */
    start = -1;
    if (length == 0)
        exit(0);            /* ^d was entered, end of user command stream */
    
    /**
     * the <control><d> signal interrupted the read system call
     * if the process is in the read() system call, read returns -1
     * However, if this occurs, errno is set to EINTR. We can check this  value
     * and disregard the -1 value
     */
    
    if ( (length < 0) && (errno != EINTR) ) {
        perror("error reading the command");
        exit(-1);           /* terminate with error code of -1 */
    }
    
    /**
     * Parse the contents of inputBuffer
     */
    
    for (i=0;i<length;i++) {
        /* examine every character in the inputBuffer */
        switch (inputBuffer[i]){
            case ' ':
            case '\t' :               /* argument separators */
                if(start != -1){
                    args[ct] = &inputBuffer[start];    /* set up pointer */
                    ct++;
                }
                inputBuffer[i] = '\0'; /* add a null char; make a C string */
                start = -1;
                break;
                
            case '\n':                 /* should be the final char examined */
                if (start != -1){
                    args[ct] = &inputBuffer[start];
                    ct++;
                }
                inputBuffer[i] = '\0';
                args[ct] = NULL; /* no more arguments to this command */
                break;
                
            default :             /* some other character */
                if (start == -1)
                    start = i;
                if (inputBuffer[i] == '&') {
                    *background  = 1;
                    inputBuffer[i-1] = '\0';
                }
        } /* end of switch */
    }    /* end of for */
    
    /**
     * If we get &, don't enter it in the args array
     */
    
    if (*background)
        args[--ct] = NULL;
    
    *size = ct;
    
    args[ct] = NULL; /* just in case the input line was > 80 */
    
    return 1;
    
} /* end of parseCommand routine */

char *findExecutable(char *path,char *exe)
{
    char *result,*seperate,*dup;
    result = (char*)malloc(128*sizeof(char));
    dup = strdup(path);
    //printf("Exec : %s\n",exe);
    while((seperate = strsep(&dup,":")) != NULL )
    {
        if(strstr(seperate,"Applications"))
        {
            continue;
        }
        strcat(result,seperate);
        strcat(result,"/");
        strcat(result,exe);
        if(access( result, F_OK ) != -1 )
        {
            //printf("EXEC : %s\n",exe);
            return result;
        }
        memset(result,0,128);
    }
    return NULL;
}

KEY_VALUE* loadBookmarks(void)
{
    FILE *ff;
    char file_path[65] = "";
    char readBuffer[256];
    char *token,*duplicate;
    KEY_VALUE *key_value = NULL;
    size_t _i = 0;
    int t = 0;
    key_value = (KEY_VALUE*)malloc(sizeof(KEY_VALUE)*16);
    key_value->_size = 16;
    key_value->_index = 0;
    strcat(file_path,getenv("HOME"));
    strcat(file_path,"/.mybookmark");
    //printf("File path : %s\n",file_path);
    ff = fopen(file_path,"r");
    if(ff)
    {
        while(fgets(readBuffer, sizeof(readBuffer), ff))
        {
            char *argsBuffer;
            argsBuffer = (char*)malloc(256*sizeof(char));
            t = 0;
            readBuffer[strcspn(readBuffer, "\n")] = 0;
            //printf("%s\n",readBuffer);
            duplicate = strdup(readBuffer);
            if(strlen(readBuffer) != 0)
            {
                while((token = strsep(&duplicate," ")) != NULL)
                {
                    //printf("Tokenizer girdi Token : %s\n",token);
                    if(t == 0)
                    {
                        key_value[_i].key = token;
                        t = 1;
                    }
                    else
                    {
                        if(t == 1)
                        {
                            strcpy(argsBuffer,token);
                        }
                        else
                        {
                            strcat(argsBuffer," ");
                            strcat(argsBuffer,token);
                        }
                        t++;
                        key_value[_i].value = argsBuffer;
                    }
                }
                //printf("Key : %s | Value : %s | i : %zu\n",key_value[_i].key,key_value[_i].value,_i);
                _i++;
                
                if(_i >= key_value->_size)
                {
                    key_value->_size = key_value->_size * 2;
                    key_value = (KEY_VALUE*)realloc(key_value,key_value->_size);
                }
            }
        }
        
        key_value->_index = _i;
        //free(argsBuffer);
        fclose(ff);
    }
    return key_value;
}

