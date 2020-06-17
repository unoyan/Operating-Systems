#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include "pthread_sleep.c"
#include <getopt.h>
#include <assert.h>
#include <unistd.h>
//#include <queue>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INT_MIN = 0;
//using namespace std;

typedef unsigned long planeid_t;

typedef struct plane{
    planeid_t id;
    struct timeval rt; //request time
    char status;
    struct timeval rnt; //runway time
    //pthread_cond_t pcond;
    //pthread_mutex_t plock;
} plane;
plane err;
planeid_t errs;
typedef struct Queue
{
    int capacity;
    int size;
    int front;
    int rear;
    plane *elements;
}queue;

typedef struct Queuestr
{
    int capacity;
    int size;
    int front;
    int rear;
    planeid_t *elements;
}queuestr;

struct queue* createQueue(int capacity)
{
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;
    queue->elements = (plane*) malloc(queue->capacity * sizeof(plane));
    return (struct queue *) queue;
}

int isFull(queue* queue)
{ return (queue->size == queue->capacity); }
int isEmpty(queue* queue)
{ return (queue->size == 0); }
void push(queue* queue, plane item){
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1)%queue->capacity;
    queue->elements[queue->rear] = item;
    queue->size = queue->size + 1;
    //printf("%d enqueued to queue\n", item);
}

// Function to remove an item from queue.
plane pop(queue* queue)
{
    if (!isEmpty(queue)) {

        plane item = queue->elements[queue->front];
        queue->front = (queue->front + 1) % queue->capacity;
        queue->size = queue->size - 1;
        return item;
    }else{
       return err;
    }
}
// Function to get front of queue
plane front(queue* queue)
{
    if (isEmpty(queue))
        return err;
    return queue->elements[queue->front];
}

// Function to get rear of queue
plane rear(queue* queue)
{
    if (isEmpty(queue)) //passes to isEmpty() function
        return err;
    return queue->elements[queue->rear];
}

struct queuestr* createQueuestr(int capacity)
{
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;
    queue->elements = (plane*) malloc(queue->capacity * sizeof(plane));
    return (struct queue *) queue;
}

void pushstr(queuestr* queue, planeid_t item){
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1)%queue->capacity;
    queue->elements[queue->rear] = item;
    queue->size = queue->size + 1;
    //printf("%d enqueued to queue\n", item);
}

// Function to remove an item from queue.
planeid_t popstr(queuestr* queue)
{
    if (!isEmpty(queue)) {

        planeid_t item = queue->elements[queue->front];
        queue->front = (queue->front + 1) % queue->capacity;
        queue->size = queue->size - 1;
        return item;
    }else{
        return errs;
    }
}


void display(queue* queue)
{
    int i;
    if (queue->front == - 1) //checks whether queue is empty or not
        printf("Queue is empty \n");
    else
    {
        printf("\nQueue is : "); //if queue is not empty then it executes the else part
        for (i = queue->front; i <= queue->rear; i++)
           // printf("%d ", queue->elements[i]);
        printf("\n");
    }
}

//destroys the created queue
void QueueDestroy(queue* queue)
{
    while (!isEmpty(queue))//passes to isEmpty() function
        pop(queue);

    queue->front = queue->rear = NULL;
    free(queue);//deletes the queue from memory ..now it stores garbage values
}


pthread_cond_t pcond_l; //condition variable for landing plane
pthread_cond_t pcond_d;//condition variable for departing plane
pthread_mutex_t plock;//lock for plane
struct timeval now, init;

int simulation_time;
int sleep_time;
int cnt; //counter for preventing starvation & keeping waiting time
int prob;//probability for land or take off
int nth;

queue lq; //landing queue
queue  dq; //departing queue
queue  eq; //emergency queue

queuestr lq_strs; //landing queue str
queuestr dq_strs; //departing queue str
//vector <log> logs;

void* landing(void* id); // landing function
void* departing(void* id); //departing function
void* act(void* dummy);  //air traffic control function

int pthread_sleep(int seconds); // used instead of sleep().
void print_queues(); // Provides to print logs that kept.
//inline int cmp_times(struct timeval t1, struct timeval t2){ return t1.tv_sec - t2.tv_sec; }

int main(int argc, char *argv[])
{
    simulation_time = 200;
    sleep_time = 2;
    pthread_mutex_init(&plock, NULL);
    pthread_cond_init(&pcond_d, NULL);
    pthread_cond_init(&pcond_l, NULL);

    id_t plane_id = 0;
    gettimeofday(&init, NULL);
    gettimeofday(&now, NULL);
    prob = 50;
    nth = 0;

    for(int ct=1; ct<argc; ct++){
        if(strcmp(argv[ct],"-s")==0){
            simulation_time = atoi(argv[ct]);
        }else if(strcmp(argv[ct],"-p")==0){
            prob = atoi(argv[ct])*100;
        }else if(strcmp(argv[ct],"-n")==0){
            nth = atoi(argv[ct]);
        }
    }

    pthread_t acthread; //air traffic control thread
    pthread_t threads[simulation_time/sleep_time];
    // pthread_t *threads = new pthread_t[simulation_time/sleep_time];
    //  plane *planes = new plane[simulation_time/sleep_time];

    pthread_create(&acthread, NULL, act,(void*) NULL);

    while (now.tv_sec <= init.tv_sec + simulation_time) {
        //    printf("now: %d\n", now.tv_sec);

        //pthread_mutex_lock(&plock);
        //for (int i = 0; i < lq.size(); i++) {

        //}
        //pthread_mutex_unlock(&plock);


        srand(time(NULL));
        int res = rand() % 100;
        //Emergency

        int tdelta = now.tv_sec - init.tv_sec;
        if (tdelta%(2*sleep_time) == 0) cnt++;


        //printf("tdelta: %d\n", tdelta);
        if (tdelta / sleep_time == 40) {//emergency landing
            pthread_mutex_lock(&plock);
            plane p;
            p.rt = now;
            push(&eq,p);
            plane_id++;

            if((now.tv_sec - init.tv_sec) >= nth)
                print_queues();

            pthread_mutex_unlock(&plock);
        }
        else if (res <= prob) { //landing
            //printf("%s%d\n\n", "Landing plane with id", plane_id);
            pthread_create(&threads[plane_id], NULL, landing, (void*)plane_id);
            plane_id++;
        }
        else if (res <= 100) { //departure
            //printf("%s%d\n\n", "Departing plane with id", plane_id);
            pthread_create(&threads[plane_id], NULL, departing, (void*)plane_id);
            plane_id++;
        } else {
            pthread_mutex_lock(&plock);
            if((now.tv_sec - init.tv_sec) >= nth)
                print_queues();

            pthread_mutex_unlock(&plock);
        }


        //else printf("Nothing happened\n\n");

        pthread_sleep(sleep_time);
        gettimeofday(&now, NULL);
    }

    //pthread_join(acthread, NULL);
    //for (int i = 0; i < plane_id; i++) {
    //  pthread_join(threads[i], NULL);
    //}

    pthread_mutex_destroy(&plock);
    pthread_cond_destroy(&pcond_l);
    pthread_cond_destroy(&pcond_d);
    //delete [] threads;
    return 0;
}

inline void log_print(plane p) { printf("%20d %20c %20d %20d %20d\n", p.id, p.status, p.rt.tv_sec - init.tv_sec,
                                        p.rnt.tv_sec - init.tv_sec, p.rnt.tv_sec - p.rt.tv_sec); }

inline void log_title() { printf("%20s %20s %20s %20s %20s\n%s\n", "PlaneID", "Status",
                                 "Request Time", "Runway Time", "Turnaround Time",
                                 "---------------------------------------------------------------------------------------------------------------------------");
}
/*
void print_queues()
{
    printf("On Air: ");
    for (int i = 0; i < lq.size(); i++) {
        planeid_t id = lq_strs.front();
        printf("%d,", id);
        popstr(lq_strs);
        pushstr(lq_strs,id);
    }printf("\n");
    printf("On Pist: ");
    for (int i = 0; i < dq.size; i++) {
        planeid_t id = dq_strs.front();
        printf("%d,", id);
        dq_strs.pop();
        dq_strs.push(id);
    }printf("\n");
}
*/

void* act(void *dummy)
{
    // wait for signal from the first plane
    log_title();
    while (now.tv_sec <= init.tv_sec + simulation_time) {
        //printf("lq size:%d\n", lq.size());
        //printf("dq size: %d\n", dq.size());
        //printf("eq size: %d\n", eq.size());
        //emergency landing
        pthread_mutex_lock(&plock);

        if (!isEmpty(&eq)) {
            //printf("Emergency landing occured\n\n");
            pop(&eq);
        }
        else if(!isEmpty(&lq)//condition 2.a
                && dq.size < 5 //condition 2.b
                &&  !(!isEmpty(&dq) && ((now.tv_sec - front(&dq).rt.tv_sec) >= 10*sleep_time))//condition 2.c
                || lq.size > dq.size //a 4th condition to prevent landing starvation by disallowing existence of a longer landing queue than departure queue
                ) {
            plane p = front(&lq);
            //printf("Signaling for landing %d\n\n", p.id );
            pop(&lq);
            popstr(&lq_strs);
            pthread_cond_signal(&pcond_l);
        }

        else if(!isEmpty(&dq)) {

            plane p = front(&dq);
            //printf("Signaling for departing %d\n\n", p.id );
            pop(&dq);
            popstr(&dq_strs);
            pthread_cond_signal(&pcond_d);
        } //else printf("no act\n\n");
        pthread_mutex_unlock(&plock);
        pthread_sleep(2*sleep_time);
        //gettimeofday(&now, NULL);
    }
    pthread_exit(NULL);
}


void* landing(void* id)
{
    plane p;
    p.id = (planeid_t)id;
    gettimeofday(&(p.rt), NULL);
    p.status = 'L';
    pthread_mutex_lock(&plock);
    //printf("I locked for land: %d\n\n", p.id);
    //printf("Pushing to queue %d \n", p.id);
    push(&lq,p);
    pushstr(&lq_strs,p.id);
    //  printf("Pushed to queue to land %d --  \n\n", p.id);//, lq.back().id);
    if((now.tv_sec - init.tv_sec) >= nth)
        print_queues();

    pthread_cond_wait(&pcond_l, &plock);
    gettimeofday(&(p.rnt), NULL);
    //printf("here\n");
    log_print(p);

    pthread_mutex_unlock(&plock);
    // pthread_cond_init(&(p.pcond), NULL);
    //pthread_cond_wait(&(p.pcond), &plock);
    //printf("Eheeey: %ld\n\n", p.id);
    //pthread_cond_destroy(&(p.pcond));

    pthread_exit(NULL);
}

void* departing(void* id)
{
    plane p;
    p.id = (planeid_t)id;
    gettimeofday(&(p.rt), NULL);
    p.status = 'D';
    pthread_mutex_lock(&plock);
    //if(tdelta>=nth)
    //    print_queues();
    //printf("I locked depart: %d\n\n", p.id);
    push(&dq,p);
    pushstr(&dq_strs,p.id);
    //printf("Pushed to queue depart  %d --  \n\n", p.id);//lq.back().id);
    //cout << dq_strs << "\n";

    pthread_cond_wait(&pcond_d, &plock);
    gettimeofday(&(p.rnt), NULL);
    //  printf("there\n");
    log_print(p);
    //  printf("Pushing to queue %d \n\n", p.id);
    //pthread_cond_init(&(p.pcond), NULL);
    //pthread_cond_wait(&(p.pcond), &plock);
    //printf("Alooha: %ld\n\n", p.id);
    //pthread_cond_destroy(&(p.pcond));
    pthread_mutex_unlock(&plock);
    pthread_exit(NULL);
}
