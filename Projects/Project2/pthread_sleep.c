{\rtf1\ansi\ansicpg1252\cocoartf2512
\cocoatextscaling0\cocoaplatform0{\fonttbl\f0\fmodern\fcharset0 Courier;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue0;}
{\*\expandedcolortbl;;\cssrgb\c0\c0\c0;}
\paperw11900\paperh16840\margl1440\margr1440\vieww10800\viewh8400\viewkind0
\deftab720
\pard\pardeftab720\partightenfactor0

\f0\fs24 \cf2 \expnd0\expndtw0\kerning0
\outl0\strokewidth0 \strokec2 \
#include <stdio.h>\
#include <stdlib.h>\
#include <pthread.h>\
#include <time.h>\
 /****************************************************************************** \
  pthread_sleep takes an integer number of seconds to pause the current thread \
  original by Yingwu Zhu\
  updated by Muhammed Nufail Farooqi\
  *****************************************************************************/\
int pthread_sleep (int seconds)\
\{\
   pthread_mutex_t mutex;\
   pthread_cond_t conditionvar;\
   struct timespec timetoexpire;\
   if(pthread_mutex_init(&mutex,NULL))\
    \{\
      return -1;\
    \}\
   if(pthread_cond_init(&conditionvar,NULL))\
    \{\
      return -1;\
    \}\
   struct timeval tp;\
   //When to expire is an absolute time, so get the current time and add //it to our delay time\
   gettimeofday(&tp, NULL);\
   timetoexpire.tv_sec = tp.tv_sec + seconds; timetoexpire.tv_nsec = tp.tv_usec * 1000;\
\
   pthread_mutex_lock (&mutex);\
   int res =  pthread_cond_timedwait(&conditionvar, &mutex, &timetoexpire);\
   pthread_mutex_unlock (&mutex);\
   pthread_mutex_destroy(&mutex);\
   pthread_cond_destroy(&conditionvar);\
\
   //Upon successful completion, a value of zero shall be returned\
   return res;\
\
\}\
}