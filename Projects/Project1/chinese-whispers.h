//
//  chinese-whispers.h
//  COMP304-A1
//

#ifndef chinese_whispers_h
#define chinese_whispers_h

#define SERVER_QUEUE_NAME "/s-chinese-whispers"
#define MAX_MSG 10
#define MSG_SIZE 100

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/fcntl.h>

typedef struct __struct_1
{
    char name[64];
    char in_buf[128];
    char out_buf[128];
    struct mq_attr _atr;
    mqd_t server;
    mqd_t client;

} _PROCESS;

#endif /* chinese_whispers_h */
